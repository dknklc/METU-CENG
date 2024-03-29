Name : Dekan KILIÇ
Number : 2268886

I have used Miller which is like awk, sed, cut, join, and sort for data formats such as CSV, TSV, JSON, JSON Lines, and positionally-indexed.

To install the Miller yo your own machine:
For Linux: sudo apt-get install miller      or     yum install miller 
For Windows: choco install miller

After the installation is completed , 
On terminal , write this script on the directory where songs_normalize.csv presents.This script will be generating a .tsv file named as input.tsv.

mlr --c2t --implicit-csv-header --headerless-csv-output cat songs_normalize.csv > input.tsv

Please, use input.tsv file as the input to our Hw3 in arg[1] like,

hadoop jar Hw3.jar Hw3 total input.tsv output_total

