# -*- coding: utf-8 -*-
from __future__ import unicode_literals
from django.db import models
from django.shortcuts import render,render_to_response
from django import forms
from django.http import HttpResponse,HttpResponseRedirect
from django.db import models
import django.utils.timezone as timezone
import python_mysql
import os
# Create your views here.


class UserForm(forms.Form):
	username = forms.CharField(label='用户名')
	password = forms.CharField(widget=forms.PasswordInput,required=True,label=' 密  码  ')
	
class PoliceForm(forms.Form):
	policename = forms.CharField(required=False,label='民警姓名')
	team_number = forms.IntegerField(min_value=1, max_value=4,required=False,label='外勤队编号')
	beg_date = forms.DateField(widget=forms.DateInput(attrs={'type': 'date'}),label='开始日期')
	end_date = forms.DateField(widget=forms.DateInput(attrs={'type': 'date'}),label='截止日期')
	
class TeamNum(models.Model):
	team_one = '1'
	team_two = '2'
	team_three = '3'
	team_four = '4'
	outside_work_team = (
        (team_one, '外勤一队'),
        (team_two, '外勤二队'),
        (team_three, '外勤三队'),
        (team_four, '外勤四队'),
	)
	
	policename = forms.CharField(required=False)
	outside_team = models.CharField(
		max_length=1,
		choices=outside_work_team,
		default=team_one,
		)

class UploadForm(forms.Form):
	gangbiao = forms.FileField(label='岗表')	


	
def login(req):
	if req.method == "POST":
		uf = UserForm(req.POST)
		if uf.is_valid():
			username = uf.cleaned_data['username']
			password = uf.cleaned_data['password']
#			print username, password
			response = HttpResponseRedirect('/index/')
			response.set_cookie('username',username,3600)
			response.set_cookie('password',password,3600)
			return response
	else:
		uf = UserForm()
	return render_to_response('login.html',{'uf':uf})

def index(req):	
	username = req.COOKIES.get('username','')
	password = req.COOKIES.get('password','')
	authority= python_mysql.web_Check_Name_Password(username,password)
#	print authority,password,username
	if authority==0:
		return render_to_response('wrongpassword.html',{})
	police = PoliceForm()
	upload = UploadForm()
	print username
	if req.method == "POST":
		police = PoliceForm(req.POST)
		upload = UploadForm(req.POST,req.FILES)
		if police.is_valid():
			policename = police.cleaned_data['policename']
			team_number = police.cleaned_data['team_number']
			beg_date = police.cleaned_data['beg_date']
			end_date = police.cleaned_data['end_date']
			print policename, team_number,beg_date,end_date,username
			if policename:
				response = HttpResponseRedirect('/query/')
				sum_time =  python_mysql.web_get_work_time_by_policeName(policename,beg_date,end_date)
				response.set_cookie('sum_time',sum_time,3600)
				response.set_cookie('policename',policename,36)
				response.set_cookie('beg_date',beg_date,36)
				response.set_cookie('end_date',end_date,36)
				return response
			if team_number:
				response = HttpResponseRedirect('/query/')
				sum_time = python_mysql.web_get_work_time_by_teamID(team_number,beg_date,end_date)
				response.set_cookie('sum_time',sum_time,3600)
				response.set_cookie('beg_date',beg_date,36)
				response.set_cookie('end_date',end_date,36)
				response.set_cookie('team_number',team_number,36)
				return response
		if upload.is_valid():
			print upload.cleaned_data['gangbiao'].name
			print upload.cleaned_data['gangbiao'].size
			if upload.cleaned_data['gangbiao'].size > 0:
				fp = file(upload.cleaned_data['gangbiao'].name,'wb')
				s = upload.cleaned_data['gangbiao'].read()
				fp.write(s)
				fp.close()
				python_mysql.web_analysis_xls(upload.cleaned_data['gangbiao'].name)
				os.remove(upload.cleaned_data['gangbiao'].name)
				response = HttpResponseRedirect('/finishupload/')
				return response
	return render_to_response('index.html',{'police':police,'upload':upload})

def logout(req):
	response = render_to_response('logout.html',{})
	response.delete_cookie('username')
	response.delete_cookie('password')	
	return response
	
def query(req):
	sum_time = req.COOKIES.get('sum_time','')	
	policename = req.COOKIES.get('policename','')
	beg_date = req.COOKIES.get('beg_date','')
	end_date = req.COOKIES.get('end_date','')
	team_number = req.COOKIES.get('team_number','')
	response = render_to_response('query.html',{'sum_time':sum_time,'policename':policename,'beg_date':beg_date,'end_date':end_date,'team_number':team_number})
	response.delete_cookie('sum_time')
	response.delete_cookie('policename')
	response.delete_cookie('beg_date')
	response.delete_cookie('end_date')
	response.delete_cookie('team_number')
	return response

def finishupload(req):
	sum_time = req.COOKIES.get('sum_time','')	
	return render_to_response('finishupload.html',{'sum_time':sum_time})