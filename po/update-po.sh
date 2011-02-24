#!/bin/sh

# Add the comment to the top of the file
cat > $1/POTFILES.in << EOF
# List of source files which contain translatable strings.
EOF

find src data -type f |
	grep -v '\/.svn\/' |
	grep -e '\.c\(pp\|xx\)\?$' -e 'data.*strings.*\.txt$' -e 'data.*sequenceaudio.*\.tx.$' -e '\.slo$' -e '\.rmsg$' |
	grep -v -e '\.lex\.c\(pp\|xx\)\?$' -e '\.tab\.c\(pp\|xx\)\?$' -e 'GLee\.c' -e 'src/betawidget/*' |
	sort >> $1/POTFILES.in
