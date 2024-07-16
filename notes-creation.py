#!/usr/bin/env python3

# notes-creation.py

"""
Description: Creates notes as html and generates index as well.
Author: Arun
Date created: 2024/05/16
Version: 0.1.0
Python version: 3.9.16
"""

import sys
import os
import time
import glob

def usage(script_name):
	name, ext = os.path.splitext(script_name)

	print("usage: {0}.py".format(name))
	print("")


def add_index_header(index_fp):
	header = (
		'<!DOCTYPE html>\n' +
		'<html>\n' +
		'<head>\n' +
		'<title>arung</title>\n' +
		'<link rel="stylesheet" href="notes/main.css" media="screen" /> <link rel="stylesheet" href="notes/mobile.css" media="screen and (max-device-width: 800px)" />\n' +
		'<meta name="description" content="arung: notebook">\n' +
		'</head>\n' +
		'<meta name="viewport" content="width=device-width, initial-scale=1.0,"/>\n' +
		'\n' +
		'<body link="#000099">\n' +
		'</body>\n' +
		'<body vlink=#80CCFF>\n' +
		'</body>\n' +
		'<body bgcolor=white>\n' +
		'</body>\n' +
		'<body text=black>\n' +
		'</body>\n' +
		'<h1 align="left"> <font color=#292929>not</font><font color="#8B8B8B">e&nbsp;s</font>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </h1>' +
		'\n'
	)
	index_fp.write("{0}\n".format(header))
	index_fp.write("{0}\n".format("<table>"))


def add_index_footer(index_fp):
	index_fp.write("{0}\n".format("</table>"))
	index_fp.write("{0}\n".format("</html>"))


def addto_index_html(index_fp, html_fname, note_time, note_msg):
	nmonth = int(note_time[4:6])
	nday = int(note_time[6:8])
	nyear = int(note_time[2:4])

	nhour = int(note_time[8:10])
	nminute = int(note_time[10:12])
	nmerdian = "am" if nhour < 12 else "pm"

	hour12 = nhour if nhour <= 12 else nhour - 12

	ndate = "{0}.{1}.{2}".format(nmonth, nday, nyear)
	ntime = "{0}:{1:02} {2}".format(hour12, nminute, nmerdian)

	td_line = '''<td> <a href = {0} style="text-decoration:none;"> {1} &nbsp;{2} </a><font color="white"> {3} </font></td>'''.format(
				str(html_fname),
				str(ndate),
				str(ntime),
				str(note_msg if len(note_msg) > 0 else "notes @ {0}".format(ntime))
				)

	index_fp.write("{0}\n".format("<tr>"))
	index_fp.write("{0}\n".format(td_line))
	index_fp.write("{0}\n".format("</tr>"))
	index_fp.write("\n")


def create_note_html(note_fname, html_fname):
	html_line = '<br>\n'
	html_meta = '<meta name="viewport" content="width=device-width, initial-scale=1.0,"/>'
	html_head = (
	            '<head> <link rel="stylesheet" href="main.css" media="screen" /> ' + 
	            '<link rel="stylesheet" href="mobile.css" media="screen and (max-device-width: 800px)" /> </head>'
				)

	html_content = html_line
	with open(note_fname, "r", encoding='utf-8') as rfp:
		try:
			content = rfp.readlines()
		except Exception as error:
			print("{0}: {1}".format(note_fname, error))
			sys.exit(1)

		if len(content) > 0:
			for c in content:
				c = c.strip()
				c = str(c) + str(html_line)
				html_content += c

	with open(html_fname, "w") as fp:
		fp.write('{0}\n'.format(html_meta))
		fp.write('{0}\n'.format(html_head))
		fp.write('{0}\n'.format(html_content))
		print("Convert {0} => {1}...".format(note_fname, html_fname))


def create_note_text(note_fname, html_fname):
	html_content = ''
	with open(note_fname, "r", encoding='utf-8') as rfp:
		try:
			content = rfp.readlines()
		except Exception as error:
			print("{0}: {1}".format(note_fname, error))
			sys.exit(1)

		if len(content) > 0:
			for c in content:
				html_content += c
		
	with open(html_fname, "w") as fp:
		fp.write('{0}\n'.format(html_content))
		print("Convert {0} => {1}...".format(note_fname, html_fname))
		

def conv_note2html(note_fname, index_fp):
	name, ext = os.path.splitext(note_fname)
	
	if ext == ".html":
		return

	note_tmp  = name.split("-")
	note_time = note_tmp[0][0:12]

	if note_time.isnumeric() == False:
		note_time_create = time.ctime(os.path.getmtime(note_fname))
		note_time = time.strftime("%Y%m%d%H%M", time.strptime(note_time_create))
		note_msg  = '-'.join(note_tmp[0:])
	else:
		note_msg  = '-'.join(note_tmp[1:])

	html_fname = 'notes/note' + str(note_time) + ('.html' if ext == ".txt" else '.txt')

	addto_index_html(index_fp, html_fname, note_time, note_msg)
	if ext == ".txt":
		create_note_html(note_fname, html_fname)
	else:
		create_note_text(note_fname, html_fname)


def main():
	notes_dir = 'notes'
	#notes_files = glob.glob("./*[.txt | .py]")
	notes_files = glob.glob("./*[.txt]")

	# https://techoverflow.net/2019/11/10/how-to-sort-files-by-modification-date-in-python/
	# sort by descending order
	notes_files = sorted(notes_files, key=lambda t: -os.stat(t).st_mtime)

	if not os.path.exists(notes_dir):
		try:
			os.mkdir('notes')
		except OSError as error:
			print(error)
			sys.exit(1)

	index_fp = open('index.html', 'wt')
	add_index_header(index_fp)

	for file in notes_files:
		note_fname = os.path.basename(file)
		if len(note_fname) <= 0:
			print("{0} error: invalid argument".format(note_fname))
			continue

		conv_note2html(note_fname, index_fp)

	add_index_footer(index_fp)
	index_fp.close()


if __name__ == "__main__":
	main()
