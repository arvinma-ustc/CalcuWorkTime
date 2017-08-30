#!/usr/bin/env python
#coding=utf-8

def con_db():
	db = MySQLdb.connect("localhost","mazan","ting","statistics_work_time",charset="utf8" )
	return db;
	
def close_db(db):
	db.close()


def show_all_police(db):
	# SQL 查询语句
	cursor = db.cursor()
	sql = "SELECT * FROM police "
	try:
	   # 执行SQL语句
	   cursor.execute(sql)
	   # 获取所有记录列表
	   results = cursor.fetchall()
	   for row in results:
			policeID = row[0]
			name = row[1]
			team = row[2]		
			#print the results
			print "policeID=%d,  name=%s,  team=%d" % \
				 (policeID, name, team)
				 
	except:
	# Rollback in case there is any error
		db.rollback()				 

def get_policeID(db,name):
	# SQL 查询语句
	cursor = db.cursor()
#	name=raw_input("请输入警员姓名：")
	sql = "select policeID,team from police where locate(name,'%s')" % \
		(name)
	try:
	   # 执行SQL语句
	   cursor.execute(sql)
	   # 获取所有记录列表
	   results = cursor.fetchall()
	   police_id =[]
	   police_team=[]
	   if(len(results)>0):
		   for row in results:
				police_id.append(row[0])
				police_team.append(row[1])
	   else:
			police_id.append(0)
			police_team.append(0)
	   return (police_id, police_team)
				 
	except:
	# Rollback in case there is any error
		db.rollback()
	return ([0], [0])

	
def get_positionID(db,name):
	# SQL 查询语句
	cursor = db.cursor()
	sql = "select positionID from position where name= '%s'" % \
		(name)
	try:
	   # 执行SQL语句
	   cursor.execute(sql)
	   # 获取所有记录列表
	   results = cursor.fetchall()
	   return results[0][0]
				 
	except:
	# Rollback in case there is any error
		db.rollback()
		return 0
		
def insert_police(db):
	# 使用cursor()方法获取操作游标 
	policeID=input("请输入警号：")
	name=raw_input("请输入警员姓名：")
	team=input("请输入警员所属外勤对号：")
	
	cursor = db.cursor()
		# SQL 插入语句
	sql = "INSERT INTO police(policeID, \
		name, team) \
		VALUES ('%d', '%s', '%d' )" % \
		(policeID, name, team)
	
	try:
	# 执行sql语句
		cursor.execute(sql)
	# 提交到数据库执行
		db.commit()
	except:
	# 发生错误时回滚
		db.rollback()

def insert_duty_mes(db,riqi,begin_time,positionID,policeID,duration,team):
	# SQL 查询语句
	cursor = db.cursor()
	sql = "INSERT INTO duty (riqi,begin_time,positionID,policeID,duration,team) \
		VALUES ('%s', '%s', '%d', '%d', '%f', '%d' )" % \
		(riqi,begin_time,positionID,policeID,duration,team)
	try:
	# 执行sql语句
		cursor.execute(sql)
	# 提交到数据库执行
		db.commit()
	except:
	# 发生错误时回滚
		db.rollback()


def get_time_line(table):
	nrows = table.nrows
	for i in range(nrows):		
		if "时间" in table.row_values(i)[0]:
			return i;
	
def get_position(table,begin,db):
	nrows = table.nrows
	for i in range(begin,nrows):		
#		print i
		position_name = table.row_values(i)[0]
		position_name = str(position_name)
		if position_name.strip()<>'':
			positionID = get_positionID(db,position_name)
			if positionID > 0:
#				print positionID,position_name
				return positionID,i
	return -1,nrows


def deal_one_rows(table,nrows,db,begin):
	ncols = table.ncols
	policeID=[]
	team_id=[]
	if nrows < table.nrows:
		for i in range(begin,ncols):		
			police_name = table.row_values(nrows)[i]
			police_name = str(police_name)
			police_name_1 = re.sub(r'\s', '', police_name) 
			(policeID,team_id) = get_policeID(db,police_name_1)
			if policeID[0] > 0:
				return policeID,team_id,i
	return [0],[0],ncols


def get_date_from_table(table):
	ncols = table.ncols
	nrows = table.nrows
	for i in range(nrows):
		for j in range(ncols):
			if "日期" in table.row_values(i)[j]:
				str=table.row_values(i)[j]	
				place_begin=str.find(":")
				place_nian=str.find("年")
				place_yue=str.find("月")
				place_ri=place_nian + str[place_nian:].find("日")
				nian=str[place_begin+1:place_nian].strip('_')
				yue=str[place_nian+1:place_yue].strip('_')
				ri=str[place_yue+1:place_ri].strip('_')
				date_str = '%s-%s-%s' % (nian,yue,ri)
				return date_str
	return 0-0-0
	

def get_begin_time(table,nrows,ncols):
	if ncols < table.ncols and nrows < table.nrows:
		str=table.row_values(nrows)[ncols]
		if str.strip()=='':
			str=table.row_values(nrows)[ncols-1]
		if "次日" in str:
			begin_time = '20:00:00'
			duration = 11.5
		elif "7:30" in str:		
			begin_time = '7:30:00'
			duration = 2.25
		elif "9:45" in str:		
			begin_time = '9:45:00'
			duration = 2.25
		elif "12:00" in str:		
			begin_time = '12:00:00'
			duration = 2
		elif "14:00" in str:		
			begin_time = '14:00:00'
			duration = 2
		elif "16:00" in str:		
			begin_time = '16:00:00'
			duration = 2
		elif "18:00" in str:		
			begin_time = '18:00:00'
			duration = 2
		else:
			begin_time = '00:00:00'
			duration = 0
		return begin_time,duration
	return '00:00:00',0
	

def analysis_xls(file_name,db):
	data = xlrd.open_workbook(file_name)
	worksheets = data.sheet_names()
	for worksheet_name in worksheets:
		worksheet = data.sheet_by_name(worksheet_name)
		table = worksheet
#	table = data.sheets()[0]
		riqi = get_date_from_table(table)
		time_line=get_time_line(table)
		ncols_max = table.ncols
		nrows_max = table.nrows
		nrows = 0
		next_ncols=0
#		print "here for analysis xls",ncols_max,nrows_max
		while nrows < nrows_max:
			(positionID,nrows)=get_position(table,nrows+1,db)
			next_ncols=0
			while next_ncols < ncols_max:
				(policeID,team_id,next_ncols)=deal_one_rows(table,nrows,db,next_ncols+1)
				if policeID[0] > 0:
					(begin_time,duration) = get_begin_time(table,time_line,next_ncols)
					duration=duration/len(policeID)
					for i in range(len(policeID)):
						print riqi, policeID[i],positionID,team_id[i],begin_time,duration
						insert_duty_mes(db,riqi,begin_time,positionID,policeID[i],duration,team_id[i])

					
def get_work_time_by_policeID(db,policeID,begin_date,end_date):
	# SQL 查询语句
	cursor = db.cursor()
#	name=raw_input("请输入警员姓名：")
	sql = "SELECT sum(duration) as totaltime from duty where policeID=%d and riqi between '%s' and '%s';" % \
		(policeID,begin_date,end_date)
	try:
	   # 执行SQL语句
	   cursor.execute(sql)
	   # 获取所有记录列表
	   results = cursor.fetchall()
	   for row in results:
			totaltime = row[0]
			return totaltime				 
	except:
	# Rollback in case there is any error
		db.rollback()
	return 0

def get_work_time_by_teamID(db,teamID,begin_date,end_date):
	# SQL 查询语句
	cursor = db.cursor()
#	name=raw_input("请输入警员姓名：")
	sql = "SELECT sum(duration) as totaltime from duty where team=%d and riqi between '%s' and '%s';" % \
		(teamID,begin_date,end_date)
	try:
	   # 执行SQL语句
	   cursor.execute(sql)
	   # 获取所有记录列表
	   results = cursor.fetchall()
	   for row in results:
			totaltime = row[0]
			return totaltime				 
	except:
	# Rollback in case there is any error
		db.rollback()
	return 0
					
					
import MySQLdb
import xlrd
import sys
import re
reload(sys)
sys.setdefaultencoding('utf8')

print
print
print
# 打开数据库连接
db = con_db()

print "              欢迎进入民警工作时长统计系统"

print "此系统只根据输入岗表统计工时，由于岗表不包含路线信息，所以统计\
工时不含路线工时，不能完整反映民警工作量"
while db<>None:

	print
	print "输入 A 选择添加在库民警信息 "
	print "输入 S 显示所有在库民警信息 "
	print "输入 P 查询岗位编号 "
	print "输入 D 手动添加岗位安排信息"
	print "输入 F 从岗表添加岗位安排信息"
	print "输入 W 根据警号统计民警工时"
	print "输入 T 根据队号统计队工时"
	print "输入 Q 退出系统"
	print
	sel = raw_input("请输入你的选择：")


	if sel=='s' or sel=='S':
		show_all_police(db)
		
	elif sel =='p' or sel=='P':
		name=raw_input("请输入要查询的岗位名称：")
		get_positionID(db,name)
		
	elif sel =='d' or sel=='D':
		riqi=raw_input("请输入要日期：")
		begin_time=raw_input("请输入要时间：")
		positionID=input("请输入岗位编号：")
		team=input("请输入民警所属队号：")
		policeID=input("请输入警号：")
		duration=input("请输入持续时间：")
		insert_duty_mes(db,riqi,begin_time,positionID,policeID,duration,team)
		
	elif sel =='w' or sel=='W':
		begin_date=raw_input("请输入要开始日期：")
		end_date=raw_input("请输入要终止日期：")
		police_id=input("请输入民警警号：")
		totaltime = get_work_time_by_policeID(db,police_id,begin_date,end_date)
		if totaltime > 0:
			print ("从 %s 至 %s 民警 %d 累计工时为：%d" % (begin_date,end_date,police_id,totaltime)) 
		else:
			print ("从 %s 至 %s 无法查询到民警 %d 工时信息" % (begin_date,end_date,police_id))
		
	elif sel =='t' or sel=='T':
		begin_date=raw_input("请输入要开始日期：")
		end_date=raw_input("请输入要终止日期：")
		team_id=input("请输入外勤队编号：")
		totaltime = get_work_time_by_teamID(db,team_id,begin_date,end_date)
		if totaltime > 0:
			print ("从 %s 至 %s 外勤 %d 队累计工时为：%d" % (begin_date,end_date,team_id,totaltime)) 
		else:
			print ("从 %s 至 %s 无法查询到外勤 %d 队工时信息" % (begin_date,end_date,team_id))
		
	elif sel =='f' or sel=='F':
		file_name=raw_input("文件名（含目录）：")
		print file_name
		analysis_xls(file_name,db)
		
	elif sel=='a' or sel=='A':
		insert_police(db)
		
	elif sel=='q' or sel=='Q':
		break
		
	else:
		print "wrong input"


# 关闭数据库连接
db.close()

