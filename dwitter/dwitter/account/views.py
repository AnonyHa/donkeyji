# Create your views here.
from django.http import HttpResponse
from django.http import HttpRequest
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout
from django.utils import simplejson
from django.shortcuts import render_to_response

def handle_register(request):
	return render_to_response('register.html', {})

def handle_login(request):
	return render_to_response('login.html', {})

def handle_register2(request):
	if request.method == 'POST':
		user_name = request.POST['user_name']
		passwd = request.POST['passwd']
		user = User.objects.create_user(user_name, passwd)
		if user is not None:
			user.save()
			return HttpResponse(simplejson.dumps({'msg':'ok'}))
		else:
			return HttpResponse(simplejson.dumps({'msg':'fail'}))

def handle_login2(request):
	if request.method == 'POST':
		user_name = request.POST['user_name']
		passwd = request.POST['passwd']

		user = authenticate(username=user_name, password=passwd)
		if user is not None:
			if user.is_active():
				user.is_active = 1
				user.save()
				login(request, user)
				return HttpResponse(simplejson.dumps({'msg':'ok'}))
		else:
			return HttpResponse(simplejson.dumps({'msg':'no this user'}))

def handle_logout(request):
	logout(request)
	return HttpResponse(simplejson.dumps({'msg':'logout ok'}))

def handle_home(request):
	return render_to_response('home.html', {})
