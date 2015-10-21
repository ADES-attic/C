#!/usr/bin/env python3

import re
import subprocess
import sys

commitDate = str(subprocess.check_output(['git', 'log', '-n1', '--pretty=format:%ci']), encoding='utf-8')

status = str(subprocess.check_output(['git', 'status']), encoding='utf-8')
if not status.startswith('On branch '):
  print('Unrecognized git status:\n', status, sep='', file=sys.stderr)
  sys.exit(-1)

# extract remote from status msg
branch = status.split()[2]
m = re.search(r"'(.+)/"+branch, status)
if not m:
  print('Unrecognized git status:\n', status, sep='', file=sys.stderr)
  sys.exit(-1)
remote = m.group(1)

# lookup url for remote  
remotes = str(subprocess.check_output(['git', 'remote', '-v']), encoding='utf-8')
rw = remotes.split()
nr = len(rw)//3
for i in range(nr):
  if rw[i*3] == remote:
    break
else:
  print('Unrecognized remotes:\n', remotes, sep='', file=sys.stderr)
  sys.exit(-1)
url = rw[i+1]

# get last commit
commit = str(subprocess.check_output(['git', 'log', '-n1', '--pretty=format:%H']), encoding='utf-8')
if not ('up-to-date' in status and 'nothing to commit, working directory clean' in status):
  commit += ' + local changes'

with open('commit.h', 'w') as f:
  print(r'''#define COMMIT "{}\n\
Source: {}\n\
Branch: {}\n\
Commit: {}"'''.format(commitDate, url, branch, commit), file=f)
