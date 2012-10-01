#!/bin/sh

sentence_url='http://tatoeba.org/files/downloads/sentences.csv'
links_url='http://tatoeba.org/files/downloads/links.csv'
tags_url='http://tatoeba.org/files/downloads/tags.csv'

echo 'Starting retrieving data from tatoeba.org'
wget $sentence_url
wget $links_url
wget $tags_url
