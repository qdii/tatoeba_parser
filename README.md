tatoeba_parser
==============

DESCRIPTION
	
	tatoeba_parser is a program that parses the tatoeba database. It is helpful to retrieve all the sentences that match a given set of criterions. To make it work fully, 3 files are necessary: sentences.csv, links.csv and tags.csv. The three files can be freely retrieved from http://www.tatoeba.org .
	I first coded this program because I needed example sentences in Chinese. I wanted to translate as many as I could, but I only knew a few characters, so I had to filter out the sentences that contained unknown characters. Then I added more options and filters so as to be able to gather the sentences that had a translation into a language I knew. Then I became so proud of my level of Chinese that I decided I wanted to train my ear as well. I had to get all the sentences which were tagged as "has audio". I developped some more code about that.
	
USAGE

    Launching the program with --help should provide the list of options. I will list some example here.
    
    1. I want to retrieve all the sentences that are written in French and that have a Spanish translation
    
        parser_r --lang fra --translatable-in spa
        
    2. I want to have all the chinese sentences that are formed by a combination of the characters 你好吗
        
        parser_r --lang cmn --regex '^[你好吗]*$'
    
    3. I want to get all the sentences which translations contain the word "foo"
    
        parser_r --translation-regex '^.*foo.*$'
        
    4. I want to get all the Spanish translation tagged as "OK"

        parser_r --lang spa --has-tag "OK"
        
    5. I want to have a list of the French sentences which have no space before their ? character
    
        parser_r --lang fra --regex "^.*[a-zA-Z]\\?.*$"
        
    Some switches permit to modify the way the sentences are output. -i will write the sentences ids, and -n will write the line number.
        
AUTHOR & LICENSING

    The author is Victor Lavaud <victor.lavaud@gmail.com>, and the program / source code is under the GPL license.
