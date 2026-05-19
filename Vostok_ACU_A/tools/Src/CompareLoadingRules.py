# CompareLoadingRules.py
# Copyright (c) 2021 Whirlpool Corporation. All rights reserved -- CONFIDENTIAL.
#

# Import python libraries
import argparse
import os
import re
import sys
try:
    from tabulate import tabulate
except ImportError as e:
    print('ERROR: Tabulate package is not installed. Run "pip install tabulate" to install it.', file=sys.stderr)
    sys.exit(1)
from xml.etree import ElementTree

# Setup constants for identifying the script.
VERSION = 'v0.0.1'
PROGRAM = os.path.basename(sys.argv[0])
PROGRAM_VERSION = f'{PROGRAM} {VERSION}'
COPYRIGHT = 'Copyright (c) 2021 Whirlpool Corporation. All rights reserved -- CONFIDENTIAL'
HELP_TEXT = """\
Compare two loading rules to each other and identify the following:
    * Folders and items that are loaded by both loading rules.
        * Specifies the name of the folder or item.
    * Folders and items that are only loaded by one or the other loading rule.
        * Specifies the name of the folder or item.
        * Which loading rule loads the folder or item.
"""
HELP_EPILOG = """\
{PROGRAM_VERSION}
{COPYRIGHT}
"""


###############################################################################
##
## Parse command line parameters.
##

parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter,
                                 description=HELP_TEXT, epilog=HELP_EPILOG)
parser.add_argument('rules1Name')
parser.add_argument('rules2Name')
parser.add_argument('--verbose',
                    help='produce detailed output for debugging purposes',
                    action='store_true')
parser.add_argument('-v', '--version',
                    help=f'show version of {PROGRAM} and exit',
                    action='version',
                    version=PROGRAM_VERSION)
args = parser.parse_args()


###############################################################################
##
## Show program identification information.
##

print(PROGRAM_VERSION)
print(COPYRIGHT)


###############################################################################
##
## Define Functions
##

def parseLoadRules(filename):
    rules = {}
    root = parseXmlWithComments(filename)
    for loadrule in root:
        if args.verbose:
            if loadrule.attrib:
                print(f'  {loadrule.tag} {loadrule.attrib}')
            else:
                print(f'  {loadrule.tag}')
        rule = buildrule(loadrule)
        id = rule['id']
        rules[id] = rule
    return rules

def parseXmlWithComments(filename):
    if args.verbose:
        print(f'\nReading File: {filename}')
    xmlParser = ElementTree.XMLParser(target=ElementTree.TreeBuilder(insert_comments=True))
    tree = ElementTree.parse(filename, xmlParser)
    return tree.getroot()

def buildrule(ruleElement):
    rule = {}
    m = re.match('(item|parent)', ruleElement.tag)
    if m:
        rule['type'] = m.group(1)
    if ruleElement.attrib:
        rule['attrib'] = ruleElement.attrib.get('alternateName')
    for element in ruleElement:
        attachElementToRule(rule, element)
    setRepoPath(rule)
    setRuleId(rule)
    if args.verbose:
        print(f'    Rule {rule}')
    return rule

def attachElementToRule(rule, element):
    if 'Comment' in str(element.tag):
        parseComment(rule, element.text)
    elif element.tag == 'component':
        rule['componentId'] = element.attrib.get('itemId')
    elif element.tag == 'sandboxRelativePath':
        rule['sandboxPath'] = element.attrib.get('pathPrefix')
    elif element.tag == 'parentFolder':
        rule['folderId'] = element.attrib.get('itemId')
    elif element.tag == 'item':
        rule['itemId'] = element.attrib.get('itemId')
    elif element.tag == 'exclude':
        rule['excluded'] = []
        for excluded in element:
            rule['excluded'].append(excluded.attrib.get('name'))
    else:
        print(f'Error: Unexpected XML tag ({element.tag}) in the loading rules')
        sys.exit(1)

def parseComment(rule, comment):
    m = re.search(r'(component name|item repositoryPath|parentFolder repositoryPath)="(.*)"',
                  comment)
    if m:
        key = m.group(1)
        value = m.group(2)
        if 'component' in key:
            rule['componentName'] = value
        elif 'item' in key:
            rule['itemName'] = value
        elif 'parent' in key:
            rule['folderName'] = value

def setRepoPath(rule):
    repoPath = rule['componentName']
    if 'folderName' in rule and rule['folderName'] != '/':
        repoPath += '\n  ' + rule['folderName']
    elif 'itemName' in rule:
        repoPath += '\n  ' + rule['itemName']
    rule['repoPath'] = repoPath

def setRuleId(rule):
    id = rule['componentId']
    if 'folderId' in rule:
        id += rule['folderId']
    elif 'itemId' in rule:
        id += rule['itemId']
    rule['id'] = id

def buildTable(rules1, rules2):
    ids = getAllIds(rules1, rules2)
    table = []
    for id in ids:
        rule1 = None if id not in rules1 else rules1[id]
        rule2 = None if id not in rules2 else rules2[id]
        row = buildRow(rule1, rule2)
        table.append(row)
    table.sort(key=byComponentName)
    return table

def getAllIds(rules1, rules2):
    ids = []
    for id in rules1:
        ids.append(id)
    for id in rules2:
        if id not in ids:
            ids.append(id)
    ids.sort()
    return ids

def buildRow(rule1, rule2):
    flags = 'T' if equalProperties(rule1, rule2, 'type') else '-'
    flags += 'A' if equalProperties(rule1, rule2, 'alternateName') else '-'
    flags += 'C' if equalProperties(rule1, rule2, 'componentId') else '-'
    flags += 'I' if equalProperties(rule1, rule2, 'itemId') else '-'
    flags += 'S' if equalProperties(rule1, rule2, 'sandboxPath') else '-'
    flags += 'E' if equalProperties(rule1, rule2, 'excluded') else '-'
    flags += 'R' if equalProperties(rule1, rule2, 'repoPath') else '-'
    if rule1 and rule2:
        same = 'Same' if flags.startswith('TACISE') else 'Different'
    elif rule1:
        same = 'Only in\nRules1'
    elif rule2:
        same = 'Only in\nRules2'
                
    if rule1:
        return [same, rule1['repoPath'], flags]
    return [same, rule2['repoPath'], flags]

def equalProperties(rule1, rule2, key):
    if rule1 and rule2:
        if key in rule1 and key in rule2:
            return rule1[key] == rule2[key]
        if key in rule1 or key in rule2:
            return False
        return True
    return False

def byComponentName(row):
    return row[1]


###############################################################################
##
## Main Program
##

rules1 = parseLoadRules(args.rules1Name)
rules2 = parseLoadRules(args.rules2Name)
print(f"""
Rules1 = {args.rules1Name}
Rules2 = {args.rules2Name}

Each flag is shown when both load rules have the same corresponding value.
Note that if they both are missing the value, then they have the same value.
Flags Definitions:
  T = Type (<parentLoadRule> or <itemLoadRule>)
  A = Alternate name property
  C = Component itemId
  I = Item ID for the parentFolder or item
  S = SandboxRelativePath pathPrefix
  E = Exclude filter names
  R = Readable path (extracted from XML comments and shown in Component column)
""")
table = buildTable(rules1, rules2)
headers=['Comparison', 'Component', 'Flags']
print(tabulate(table, headers, tablefmt="fancy_grid", colalign=['center', 'left', 'center']))

sys.exit(0)

