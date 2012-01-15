# Create your views here.
from django.http import HttpResponse
from django.http import HttpRequest
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout
from django.utils import simplejson
from django.shortcuts import render_to_response, redirect

from dwitter.util.filelog import log

def handle_root(request):
	if request.user.is_authenticated():
		return redirect('/home/')
	return render_to_response('home_not_logged.html', {'logged':False})

def handle_home(request):
	if not request.user.is_authenticated():
		return redirect('/login/')
	return render_to_response('timeline.html', {'logged':True})


def handle_login(request):
	if request.user.is_authenticated():
		return redirect('/home/')
	return render_to_response('login.html', {'logged':False})

def handle_signup(request):
	if request.method == 'POST':
		name = request.POST['name']
		password = request.POST['password']
		email = 'noemail'
		user = User.objects.create_user(name, email, password)
		if user is not None:
			user.save()
			return redirect('/home/')
	return redirect('/login/')

def handle_signin(request):
	if request.method == 'POST':
		name = request.POST['name']
		password = request.POST['password']

		user = authenticate(username=name, password=password)
		if user is not None:
			if not user.is_active:
				user.is_active = 1
				user.save()
			login(request, user)
			return redirect('/home/')
	return redirect('/login/')

def handle_logout(request):
	logout(request)
	return

