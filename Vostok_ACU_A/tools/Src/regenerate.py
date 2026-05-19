"""regenerate.py

Prepare the Nucleus SDK project for building.
Copyright (c) 2020-2022 Whirlpool Corporation. All rights reserved -- CONFIDENTIAL.
"""

# Import python libraries
import argparse
import datetime
import os
import re
import sys

# Setup constants for identifying the script.
__version__ = '1.1.0'
_DESCRIPTION = """
Prepare the Nucleus SDK project for building.
    - Generate sources.mk with the following:
        - A list of source files grouped by extensions
        - Artificial targets for each module to compile and lint
        - Dependencies for each module
    - Generate paths.mk with the following:
        - A list of all paths with source or header files
This script should be run from the project's root folder.
All configured paths are relative to the project's root folder.
"""
_PROGRAM = os.path.basename(sys.argv[0])
_COPYRIGHT = 'Copyright (c) 2020-2022 Whirlpool Corporation'
_EPILOG = f"""
{_PROGRAM} v{__version__}
{_COPYRIGHT}
"""


################################################################################
# Main Script

def main():
    """Prepare a Nucleus SDK project for building"""

    # Get and validate parameters.
    args = parse_cmdline_args()
    verify_project_folder()
    verify_folders_exist(args.sources, args.verbose)

    # Evaluate project files.
    sources, headers = find_all_files(args.sources, args.srcexts,
                                      args.incdirs, args.hdrexts, args.verbose)
    if not args.quiet and not args.verbose:
        print(f'    Source files: {len(sources)}')
        print(f'    Header files: {len(headers)}')

    includes = find_direct_includes({**sources, **headers}, args.verbose)
    inccnt = sum(len(inclist) for inclist in includes.values())
    if not args.quiet and not args.verbose:
        print(f'       #includes: {inccnt}')

    dependencies = find_dependencies(sources, includes, args.verbose)
    depcnt = sum(len(deplist) for deplist in dependencies.values())
    if not args.quiet and not args.verbose:
        print(f'    Dependencies: {depcnt}')

    search_directories = find_all_search_dirs({**sources, **headers}.values(), args.incdirs,
                                              args.verbose)
    if not args.quiet and not args.verbose:
        print(f'     Search dirs: {len(search_directories)}')

    # Generate output files.
    os.makedirs(args.outdir, exist_ok=True)

    srcmk_file = write_sources_mk(args, sources, dependencies, headers)
    if not args.quiet:
        print(f'       Generated: {srcmk_file}')

    pathmk_file = write_paths_mk(args.outdir, search_directories)
    if not args.quiet:
        print(f'       Generated: {pathmk_file}')


################################################################################
# Program Functions

def parse_cmdline_args():
    """Parse the command line arguments for this application."""
    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter,
                                     description=_DESCRIPTION, epilog=_EPILOG)
    parser.add_argument('-v', '--version',
                        help=f'show version of {_PROGRAM} and exit',
                        action='version',
                        version=f'{_PROGRAM} version {__version__}')
    parser.add_argument('-s', '--sources',
                        help='space-separated list of source directories '
                             "(default: ['source'])",
                        type=str, nargs='*',
                        default=['source'])
    parser.add_argument('--srcexts',
                        help='space-separated list of source file extensions '
                             "(default: ['c', 's37'])",
                        type=str, nargs='*',
                        default=['c', 's37'])
    parser.add_argument('--hdrexts',
                        help='space-separated list of header file extensions '
                             "(default: ['h'])",
                        type=str, nargs='*',
                        default=['h'])
    parser.add_argument('-i', '--incdirs',
                        help='custom paths to add to the INCLUDES search paths '
                             "(default: [])",
                        type=str, nargs='*',
                        default=[])
    parser.add_argument('--untargets',
                        help='additional filenames that should not be made into makefile targets',
                        type=str, nargs='*',
                        default=[])
    parser.add_argument('-o', '--outdir',
                        help='path to folder to place output files '
                             '(default: config/tmp)',
                        type=str,
                        default='config/tmp')
    parser.add_argument('-q', '--quiet',
                        help='suppress most command line output',
                        action='store_true')
    parser.add_argument('--verbose',
                        help='show verbose command line output (overrides -q)',
                        action='store_true')
    args = parser.parse_args()
    args.srcexts = ['.' + ext.lstrip('.').lower() for ext in args.srcexts]
    args.hdrexts = ['.' + ext.lstrip('.').lower() for ext in args.hdrexts]
    args.untargets.extend(
        ['all', 'clean', 'clobber', 'debug', 'help', 'iap', 'objects', 'regenerate', 'release'])
    if args.verbose:
        args.quiet = False
    if not args.quiet:
        print(f'{_PROGRAM} (v{__version__})  {_COPYRIGHT}')
    return args


def is_project_name_valid(project_name):
    """Return True if the project_name conforms with Whirlpool standards."""
    if re.match(r'[A-Za-z0-9]+_(ACU|HMI|MCU|EXPACU|EXPHMI|IAP)_[!\(\)+\-.\d@\w\[\]~]+(?<!\.)$',
                project_name):
        return True
    if re.match(r'Nucleus-(Cooking|Core|Dish|Dryer|HAWasher|IAP|Refrigeration|VAWasher)-'
                r'SDK[!\(\)+\-.\d@\w\[\]~]*(?<!\.)$', project_name):
        return True
    return False


def verify_project_folder():
    """Verify that the current directory is the project folder or exit."""
    project_folder = os.path.basename(os.getcwd())
    if is_project_name_valid(project_folder):
        return
    os.chdir('..')
    parent_project_folder = os.path.basename(os.getcwd())
    if is_project_name_valid(parent_project_folder):
        return
    print(f'ERROR: "{project_folder}" is not a valid project folder name.', file=sys.stderr)
    sys.exit(2)


def verify_folders_exist(folders: list, show=False):
    """Verify that the specified folders exist or exit."""
    if not folders:
        print('ERROR: At least one source directory must be specified.', file=sys.stderr)
        sys.exit(3)
    if show:
        print(f'\n{_PROGRAM} source folders:')
    for folder in folders:
        if not os.path.isdir(folder):
            print(f'ERROR: Cannot find source folder "{folder}"', file=sys.stderr)
            sys.exit(3)
        if show:
            print(f'    {folder}')


def find_all_files(source_folders: list, srcexts: list,
                   include_folders: list, hdrexts: list, show=False) -> tuple:
    """Set the sources and headers dictionaries with all source and header files."""
    sources = {}  # dict of {filekey: filepath} for source files
    headers = {}  # dict of {filekey: filepath} for header files
    # filekey: The file's lower case filename and extension (without the directory).
    # filepath: The path of the file (including the filename and extension).

    for source_folder in source_folders:
        for root, _, files in os.walk(source_folder):
            map_files(sources, srcexts, root, files)
            map_files(headers, hdrexts, root, files)
    for include_folder in include_folders:
        for root, _, files in os.walk(include_folder):
            map_files(headers, hdrexts, root, files)
            break
    if show:
        print(f'{_PROGRAM} found {len(sources)} source files:')
        for path in sorted(sources.values(), key=str.casefold):
            print(f'    {path}')
        print(f'{_PROGRAM} found {len(headers)} header files:')
        for path in sorted(headers.values(), key=str.casefold):
            print(f'    {path}')
    return sources, headers


def find_direct_includes(files: dict, show=False) -> dict:
    """For each source and header file determine the list of directly included header files."""
    includes = {}  # dict of {filekey: headers} for all source and header files
    # filekey: The file's lower case filename and extension (without the directory).
    # headers: The list of header filekeys that are directly included by the file.
    for filekey, filepath in files.items():
        update_includes(includes, filekey, filepath)
    if show:
        print(f'{_PROGRAM} direct includes:')
        for filekey, incfiles in sorted(includes.items()):
            print(f'    {filekey} -> {sorted(incfiles)}')
    return includes


def map_files(filemap: dict, extensions: list, root: str, files: list):
    """Filter files by extension and add them to the dict of mapped files."""
    for file in files:
        filekey = file.lower()
        filepath = os.path.join(root, file).replace('\\', '/')
        _, extension = os.path.splitext(file)
        extension = extension.lower()
        if extension not in extensions:
            continue
        if filekey not in filemap:
            filemap[filekey] = filepath
        else:
            print(f'WARNING: Found multiple files with the name "{file}"\n'
                  f'         1st: "{filemap[filekey]}" will be used\n'
                  f'         2nd: "{filepath}" will be ignored', file=sys.stderr)


def update_includes(includes, filekey, filepath):
    """Associate the file with all of its #include statements."""
    with open(filepath) as fin:
        data = fin.read()
    # Groups:                                  1- -1
    incfiles = re.findall(r'^\s*#\s*include\s+"(.*?)"', data, flags=re.MULTILINE)
    if incfiles:
        incfiles = [os.path.basename(incfile).lower() for incfile in incfiles]
        includes[filekey] = sorted(incfiles)


def find_dependencies(sources, includes, show=False) -> dict:
    """For each source file determine all header file dependencies."""
    all_dependencies = {}  # dict of {filekey: headers} for all nested header file dependencies
    # filekey: The source file's lower case filename and extension (without the directory).
    # headers: The set of header filekeys that are directly or indirectly #included by filekey
    for source_filekey in sources:
        file_dependencies = set()
        considered_headers = []
        add_dependencies(file_dependencies, considered_headers, source_filekey, includes)
        all_dependencies[source_filekey] = file_dependencies
    if show and all_dependencies:
        print(f'{_PROGRAM} source dependencies:')
        for source_filekey, file_dependencies in sorted(all_dependencies.items()):
            print(f'    {source_filekey} -> {sorted(file_dependencies)}')
    return all_dependencies


def add_dependencies(file_dependencies, considered_headers, filekey, includes):
    """Build up all nested dependencies for a specific file."""
    considered_headers.append(filekey)
    if filekey not in includes:
        return
    direct_dependencies = includes[filekey]
    file_dependencies.update(direct_dependencies)
    for header_filekey in direct_dependencies:
        if header_filekey not in considered_headers:
            add_dependencies(file_dependencies, considered_headers, header_filekey, includes)


def find_all_search_dirs(files: list, extra_include_dirs: list, show=False) -> set:
    """Create a list of directories to use as include paths."""
    search_directories = set(os.path.dirname(file).replace('\\', '/') for file in files)
    search_directories.update(incdir.replace('\\', '/') for incdir in extra_include_dirs)
    if show and search_directories:
        print(f'{_PROGRAM} search directories:')
        for folder in sorted(list(search_directories), key=str.casefold):
            print(f'    {folder}')
    return search_directories


# ------------------------------------------------------------------------------
# Functions for writing the generated files

def write_sources_mk(args, sources, dependencies, headers) -> str:
    """Write the sources.mk file used by the Makefile."""
    filename = os.path.join(args.outdir, 'sources.mk').replace('\\', '/')
    with open(filename, 'w') as fout:
        write_file_header('Makefile with source files and their dependencies', fout)
        write_sources_by_ext(sources, args.srcexts, fout)
        print('\n\n\n# Define source file development targets'
              ' and define object file dependency rules.', file=fout)
        for filekey, path in sorted(sources.items()):
            write_source_target(path, args.untargets, fout)
            write_source_deps(filekey, path, dependencies, headers, fout)
        print(file=fout)
    return filename


def write_file_header(description, fout):
    """Write the header text for a generated makefile."""
    print(f'# {description}', file=fout)
    now = datetime.datetime.now()
    date = now.strftime('%A, %d %b %Y')
    time = now.strftime('%I:%M:%S %p')
    print(f'# Generated by {_PROGRAM} on {date} at {time}', file=fout)


def write_sources_by_ext(sources, srcexts, fout):
    """Write a definition for each list of source files grouped by extension."""
    for extension in srcexts:
        ext = extension[1:].upper()
        srcs = []
        for filekey, path in sources.items():
            if filekey.endswith(extension):
                srcs.append(os.path.basename(path))
        if len(srcs) > 0:
            srcs_list = ' '.join(sorted(srcs, key=str.casefold))
            fout.write(f'\n\n# Define all project {ext} files.\n{ext}FILES:={srcs_list}')


def write_source_target(path, exceptions, fout):
    """Write a target for the source file specified by path."""
    srcfile = os.path.basename(path)
    srcbase, srcext = os.path.splitext(srcfile)
    untarget = '_' if srcbase.lower() in exceptions else ''
    fout.write(f'\n\n{srcbase}{untarget} :: {srcbase}.obj')
    if srcext.lower() in ['.c', '.cpp']:
        fout.write(f' {srcbase}.lnt')


def write_source_deps(filekey, path, dependencies, headers, fout):
    """Write the dependencies for the specified object file."""
    srcfile = os.path.basename(path)
    srcbase, _ = os.path.splitext(srcfile)
    fout.write(f'\n{srcbase}.obj : {srcfile}')
    for incfile in sorted(dependencies[filekey]):
        if incfile in headers:
            hdrfile = os.path.basename(headers[incfile])
            fout.write(f' \\\n {hdrfile}')


def write_paths_mk(outdir, pathdirs):
    """Write the paths.mk file used by the Makefile."""
    outfile = os.path.join(outdir, 'paths.mk').replace('\\', '/')
    with open(outfile, 'w') as file:
        write_file_header('Makefile with directory lists', file)
        file.write('\n\n# Define directories with project source and header files.')
        write_path_dirs(pathdirs, file)
        file.write('\n')
    return outfile


def write_path_dirs(pathdirs, fout):
    """Write a definition for the list of paths used by the project."""
    incdirs_list = ' '.join(sorted(pathdirs, key=str.casefold))
    fout.write(f'\nPATHS:={incdirs_list}')


if __name__ == "__main__":
    main()
    sys.exit(0)
