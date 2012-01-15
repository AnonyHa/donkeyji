from django.db import models

# Create your models here.
class User(models.Model):
	user_name = models.CharField(max_length=32, unique=True)
	nickname = models.CharField(max_length=32)
	follow_cnt = models.IntegerField()
	following_cnt = models.IntegerField()
	dwtter_cnt = models.IntegerField()
