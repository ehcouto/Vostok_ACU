"""SortLoadingRule.py -- Script for sorting RTC loading rules.

Copyright (c) 2020-2021 Whirlpool Corporation. All rights reserved -- CONFIDENTIAL.
"""

import argparse
import os
import re
import shutil
import sys

__version__ = '0.3.0'

_PROGRAM = os.path.basename(sys.argv[0])


class LoadRuleFormatError(Exception):
    """Unable to find the project folder in the loading rule."""


def main():
    """Sort the listed components in an RTC loading rule."""
    try:
        # Parse the command line arguments.
        args = _parse_cmdline()
        _validate_loadrule_name(args.loadrule)
        variant_folder = _get_variant_folder(args.loadrule, args.variant)

        # Read the file into memory.
        with open(args.loadrule) as fin:
            contents = fin.read()

        # Set the new project directory (if requested).
        if variant_folder:
            contents = _update_path_prefix(contents, variant_folder)

        # Extract the XML header/footer.
        header, components, footer = _extract_components(contents)

        # Backup the original file (if requested).
        if args.backup:
            shutil.copy(args.loadrule, args.loadrule + '.bak')

        # Create the output.
        outfile = _get_outfile_filename(args.loadrule, variant_folder)
        with open(outfile, 'w') as fout:
            print(header, file=fout)
            for component in components:
                print(component, file=fout)
            print(footer, file=fout)
    except LoadRuleFormatError as error:
        # print(error, file=sys.stderr)
        sys.exit(error)
    except ValueError as error:
        # print(error, file=sys.stderr)
        sys.exit(error)
    except FileNotFoundError as error:
        # print(error, file=sys.stderr)
        sys.exit(error)


def _parse_cmdline():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description="""
Sorts components in a loading rule file to simplify file comparisons.
CAUTION: Overwrites the original loading rule.
""",
        epilog=f"""
{_PROGRAM} v{__version__}
Copyright (c) 2020-2021 Whirlpool Corporation. All rights reserved -- CONFIDENTIAL
""")
    parser.add_argument('loadrule',
                        metavar="LOADRULE",
                        help='the filename of the loading rule to sort')
    parser.add_argument('-v', '--version',
                        action='version', version=f'{_PROGRAM} version {__version__}',
                        help=f'show version and exit')
    parser.add_argument('-b', '--backup',
                        action='store_true',
                        help='backup the original loading rule')
    parser.add_argument('variant',
                        metavar="VARIANT", nargs='?',
                        help='append VARIANT to the loading rule project directory')
    args, extra = parser.parse_known_args()
    if extra:
        if args.variant or len(extra) > 1:
            raise ValueError(f'unrecognized arguments: {extra}')
        args.variant = extra[0]
    return args


def _validate_loadrule_name(loadrule: str):
    """Verify that the loadrule name complies with the naming standards."""
    name, _ = os.path.splitext(os.path.basename(loadrule))
    if re.match(r'[A-Za-z0-9]+_(ACU|HMI|MCU|EXPACU|EXPHMI|IAP)_'
                r'[^\x00- "\*/:<>?\\\|\x7F]+$', name):
        return
    if re.match(r'Nucleus-(Cooking|Core|Dish|Dryer|HAWasher|IAP|Refrigeration|VAWasher)-SDK'
                r'[^\x00- "\*/:<>?\\\|\x7F]*$', name):
        return
    raise ValueError(f'The load rule name ("{name}") does not '
                     'conform to the naming standards.')


def _get_variant_folder(loadrule: str, variant: str) -> str:
    """Return the name of the folder for the variant loading rule."""
    if variant is None:
        return ''
    name, _ = os.path.splitext(os.path.basename(loadrule))
    folder = name + variant
    if not re.fullmatch(r'[^\x00- "\*/:<>?\\\|\x7F]+', variant):
        raise ValueError(f'The variant folder name ("{folder}") does not '
                         'conform to the naming standards.')
    return folder


def _update_path_prefix(contents: str, variant_folder: str) -> str:
    """Update the load rule file contents to use the new project directory."""
    found = re.search(r'pathPrefix="/([^ \\/]+)/', contents)
    if not found:
        raise LoadRuleFormatError('Cannot find the original project folder in the loading rule')
    pathprefix = found[1]
    return contents.replace(pathprefix, variant_folder)


def _extract_components(contents: str) -> tuple:
    """Extract the parts of the loading rule: header, components, and footer."""
    # Groups:       header 1- -1                           footer 2- -2
    found = re.fullmatch(r'(.*?) *<\w+LoadRule.*<\/\w+LoadRule>\s+(.*?)', contents, re.DOTALL)
    if not found:
        raise LoadRuleFormatError('Cannot extract loading rule components')
    header = found[1].strip()
    footer = found[2].strip()
    components = re.findall(r'^ *<(?:item|parent)LoadRule.*?<\/(?:item|parent)LoadRule>',
                            contents, re.MULTILINE | re.DOTALL)
    components.sort(key=_byname)
    return header, components, footer


def _byname(component_text):
    """Determine the sort key for the component text."""
    # Groups:                           1- -1    2-               -2                 3- -3
    found = re.search(r'component name="(.+?)".*?(parentFolder|item) repositoryPath="(.*?)"',
                      component_text, flags=re.MULTILINE | re.DOTALL)
    if not found:
        return ''
    if found[3] == '/':
        return found[1].lower()
    return found[1].lower() + found[3].lower()


def _get_outfile_filename(loadrule: str, folder: str) -> str:
    """Get the name of the loading rule to create."""
    if folder:
        return os.path.join(os.path.dirname(loadrule), folder + '.loadrule')
    return loadrule


if __name__ == "__main__":
    main()
    sys.exit(0)
