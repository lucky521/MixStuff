rm -rf .git/refs/original/
git reflog expire --expire=now --all
git gc --prune=now
git gc --aggressive --prune=now

git filter-branch --index-filter 'git rm --cached --ignore-unmatch ./code2012/refresh_nkbt.py' --prune-empty --tag-name-filter cat -- --all

git push origin master --force
