"""sr_revision.py

Calculate the safety-related Class B signature for the project.
Copyright (c) 2021-2022 Whirlpool Corporation. All rights reserved -- CONFIDENTIAL.
"""

# Import python libraries
import argparse
import datetime
import hashlib
import os
import re
import sys

# Setup constants for identifying the script.
__version__ = '2.1.0'
_DESCRIPTION = """
Calculate the safety-related Class B signature for the project.
   1. Find all source files in the path trees specified by the -s option
      Source files determined by the '-e' command line option
   2. Calculate SHA256 hash on all files with the "@defgroup CLASS_B" tag
   3. Calculate SHA256 hash on all files specified with the -f option
   4. Create/overwrite the signature file specified with the -o option
      Only overwrite the signature file if there is a change
This script should be run from the project's root folder.
All paths are relative to the project's root folder.
"""
_PROGRAM = os.path.basename(sys.argv[0])
_COPYRIGHT = 'Copyright (c) 2021-2022 Whirlpool Corporation'
_EPILOG = f"""
{_PROGRAM} v{__version__}
{_COPYRIGHT}
"""


################################################################################
# Main Script

def main():
    """Calculate the safety-related Class B signature for the project."""

    # Get and validate parameters.
    args = parse_cmdline_args()
    verify_project_folder()
    verify_folders_exist(args.sources, args.verbose)
    verify_signature_file_path(args.output, args.verbose)
    verify_extra_files(args.files, args.verbose)

    # Evaluate project files.
    sources = find_all_files(args.sources, args.exts, [args.output], args.verbose)
    srfiles = find_srfiles(sources, args.files, args.verbose)
    project_hash = calculate_project_hash(srfiles, args.verbose)
    if not args.quiet:
        hash_text = project_hash.hexdigest().upper()
        print(f'    SHA256: {hash_text}')

    # Generate output file.
    if signature_file_needs_update(args.output, project_hash, args.verbose):
        write_signature_file(args.output, sources, srfiles, project_hash)
        print(f'    {args.output} was updated.')
    else:
        print(f'    {args.output} was not updated.')


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
    parser.add_argument('-e', '--exts',
                        help='space-separated list of file extensions '
                             "(default: ['c', 'h', 's37'])",
                        type=str, nargs='*',
                        default=['c', 'h', 's37'])
    parser.add_argument('-f', '--files',
                        help='space-separated list of extra files to hash '
                             "(default: ['config/project_sr_config.mk', "
                             "'tools/common_sr_config.mk'])",
                        type=str, nargs='*',
                        default=['config/project_sr_config.mk', 'tools/common_sr_config.mk'])
    parser.add_argument('-o', '--output',
                        help='path of the generated signature file (default: '
                             "'source/ProjectSpecific/Common/all/SRConfig/ClassB_Signature.h')",
                        type=str, nargs='?',
                        default='source/ProjectSpecific/Common/all/SRConfig/ClassB_Signature.h')
    parser.add_argument('-q', '--quiet',
                        help='suppress most command line output',
                        action='store_true')
    parser.add_argument('--verbose',
                        help='show verbose command line output (overrides -q)',
                        action='store_true')
    args = parser.parse_args()
    args.exts = ['.' + ext.lstrip('.').lower() for ext in args.exts]
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


def verify_signature_file_path(signature_file, show=False):
    """Verify that the signature file has a valid path or exit."""
    sigdir = os.path.dirname(signature_file)
    if not os.path.isdir(sigdir):
        print(f'ERROR: The folder for the signature file "{sigdir}" must exist.', file=sys.stderr)
        sys.exit(4)
    if show:
        print(f'{_PROGRAM} class B signature file:')
        print(f'    {signature_file}')


def verify_extra_files(extra_files, show=False):
    """Verify that the extra files to include in the SHA256 exist or exit."""
    if show and extra_files:
        print(f'{_PROGRAM} extra files:')
    for file in extra_files:
        if not os.path.isfile(file):
            print(f'ERROR: Cannot find the extra safety-related file "{file}"', file=sys.stderr)
            sys.exit(5)
        if show:
            print(f'    {file}')


def find_all_files(source_folders: list, extensions: list, ignore_files: list, show=False) -> dict:
    """Create a dictionary of all source files to scan."""
    sources = {}  # dict of {filekey: filepath} for source and header files
    # filekey: The source file's lower case filename and extension (without the directory).
    # filepath: The path of the file (including the filename and extension).

    ignore = [os.path.basename(path).lower() for path in ignore_files]
    for source_folder in source_folders:
        for root, _, files in os.walk(source_folder):
            for file in files:
                filekey = file.lower()
                if filekey in ignore:
                    continue
                filepath = os.path.join(root, file).replace('\\', '/')
                _, extension = os.path.splitext(file)
                extension = extension.lower()
                if extension not in extensions:
                    continue
                if filekey not in sources:
                    sources[filekey] = filepath
                else:
                    print(f'WARNING: Found multiple files with the name "{file}"\n'
                          f'         1st: "{sources[filekey]}" will be used\n'
                          f'         2nd: "{filepath}" will be ignored', file=sys.stderr)
    if show:
        print(f'{_PROGRAM} found {len(sources)} source files:')
        for path in sorted(sources.values(), key=str.casefold):
            print(f'    {path}')
    return sources


def find_srfiles(sources: list, extra_files: list, show=False) -> dict:
    """Return a dictionary {filekey: sha256} with all safety-related files."""
    re_sr = re.compile(r'\@\s*defgroup\s+CLASS_B\s*$', re.M)
    srfiles = {}
    for extra in extra_files:
        filekey = os.path.basename(extra).lower()
        sources[filekey] = extra
        with open(extra, 'rb') as fin:
            data = fin.read()
        srfiles[filekey] = hashlib.sha256(data)
    for filekey, path in sources.items():
        with open(path) as fin:
            text = fin.read()
        if re_sr.search(text):
            with open(path, 'rb') as fin:
                data = fin.read()
            if filekey in srfiles:
                file = os.path.basename(path)
                print(f'WARNING: Found multiple safety-related files with the name "{file}"\n'
                      f'         1st: "{sources[filekey]}" will be used\n'
                      f'         2nd: "{path}" will be ignored', file=sys.stderr)
            srfiles[filekey] = hashlib.sha256(data)
    if show:
        print(f'{_PROGRAM} found {len(srfiles)} safety-related Class B source files:')
        for filekey in sorted(srfiles):
            filename = os.path.basename(sources[filekey])
            file_hash = srfiles[filekey].hexdigest().upper()
            print(f'    SHA256: {file_hash} {filename}')
    return srfiles


def calculate_project_hash(srfiles, show=False):
    """Calculate the project SHA256 hash from all the file hashes."""
    proj_hash = hashlib.sha256()
    for _, file_hash in sorted(srfiles.items()):
        proj_hash.update(file_hash.digest())
    if show:
        print(f'{_PROGRAM} calculated project Class B signature:')
    return proj_hash


def signature_file_needs_update(filename: str, project_hash, show=False) -> bool:
    """Determine if the project's signature file needs to be updated."""
    hexadecimal_values = ','.join([f'0x{value:02X}' for value in project_hash.digest()])
    if os.path.isfile(filename):
        with open(filename) as fin:
            contents = fin.read()
        if show:
            print(f'{_PROGRAM} searched existing signature file for signature line:')
            print(f'    {hexadecimal_values}')
        if re.search(r'^#define CLASSB_SIGNATURE {' + hexadecimal_values + r'}$',
                     contents, flags=re.MULTILINE):
            return False
    elif show:
        print(f'{_PROGRAM} found no existing signature file.')
    return True


def write_signature_file(filename, sources, srfiles, project_hash):
    """Create or overwrite the Class B signature file."""
    hexadecimal_values = ','.join([f'0x{value:02X}' for value in project_hash.digest()])
    dirname = os.path.dirname(filename)
    if not os.path.isdir(dirname):
        os.makedirs(dirname)
    with open(filename, 'w') as fout:
        now = datetime.datetime.now()
        date = now.strftime('%A, %d %b %Y')
        time = now.strftime('%I:%M:%S %p')
        guard = os.path.basename(filename).upper().replace('.', '_')
        print(f"""\
/**
*  @file
*  @brief      This file contains the Class B Signature for this project.
*
*  @details    The file was automatically generated by {_PROGRAM}
*              Generated on: {date} at {time}
*
*  @copyright  {_COPYRIGHT}
*/

#ifndef {guard}_
#define {guard}_


// Class B files in this project ({len(srfiles)}):""", file=fout)

        for filekey, file_hash in sorted(srfiles.items()):
            hash_text = file_hash.hexdigest().upper()
            filename = os.path.basename(sources[filekey])
            print(f'//  SHA256: {hash_text} {filename}', file=fout)
        hash_text = project_hash.hexdigest().upper()
        print(f"""

// Class B signature for the full project
//  SHA256: {hash_text}
#define CLASSB_SIGNATURE {{{hexadecimal_values}}}


#endif      // {guard}_""", file=fout)


if __name__ == '__main__':
    main()
    sys.exit(0)
