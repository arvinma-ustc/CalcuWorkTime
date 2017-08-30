#!/usr/bin/env python
#coding=utf-8



def get_time_line(table):
	nrows = table.nrows
	for i in range(nrows):		
		if "时间" in table.row_values(i)[0]:
			return i;
	
def get_position(table,begin):
	nrows = table.nrows
	for i in range(begin,nrows):		
		if "时间" in table.row_values(i)[0]:
			return i;			


import xlrd
import sys
reload(sys)
sys.setdefaultencoding('utf8')

data = xlrd.open_workbook('/home/mazan/code/calcu_work_time/worktable/岗二岗表7月12日.xls')
table = data.sheets()[0]

print get_time_line(table)






/home/mazan/code/calcu_work_time/worktable/值班岗表8月15日.xls