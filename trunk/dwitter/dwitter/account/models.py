from django.db import models

# Create your models here.
class Role(models.Model):
	role_name = models.CharField(max_length=32, unique=True)

class User(models.Model):
	user_name = models.CharField(max_length=32, unique=True)
	nickname = models.CharField(max_length=32)
