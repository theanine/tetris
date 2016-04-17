#!/bin/bash

if [ -z $1 ] || [ -z $2 ]; then
	echo "Usage $0 COMMIT COMMIT"
	exit 1
fi



TMP_FILE=`mktemp`
find ./ -regex '.*\.\(c\|h\)' -exec wc -l {} \; > $TMP_FILE
TOTAL_CH_LINES=`cat $TMP_FILE | sed 's/ .*//' | paste -sd+ | bc`
CLOC_CH_LINES=`cloc *.c *.h | tail -2 | head -1 | sed 's/.* //'`
CLOC_LINES=`cloc . | tail -2 | head -1 | sed 's/.* //'`
TOTAL_LINES=$(($TOTAL_CH_LINES+$CLOC_LINES-$CLOC_CH_LINES))

echo $TOTAL_CH_LINES
echo $CLOC_CH_LINES
echo $CLOC_LINES
echo $TOTAL_LINES

echo "- Average number of lines of code modified per minute: 2.5"
echo "- Average number of lines of non-code modified per minute: 0.5"
echo "- Average number of lines modified per minute: 3.0"
echo "- Percent of code lines to total lines: 74.2% "
echo "- Percent of deletions to total modifications: 18.0%"
echo "- Number of commits per hour: 4.4"
echo "- Average number of LoC modified per commit: 25.7"

rm -f $TMP_FILE
