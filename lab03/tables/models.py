# This is an auto-generated Django model module.
# You'll have to do the following manually to clean this up:
#   * Rearrange models' order
#   * Make sure each model has one field with primary_key=True
#   * Make sure each ForeignKey and OneToOneField has `on_delete` set to the desired behavior
#   * Remove `managed = False` lines if you wish to allow Django to create, modify, and delete the table
# Feel free to rename the models, but don't rename db_table values or field names.
from django.db import models

class User(models.Model):
    class Meta:
        db_table = 'users'
    id = models.IntegerField(primary_key=True)
    login = models.TextField()
    password = models.TextField()
    role = models.TextField()
    moderator_id = models.ForeignKey('User', models.CASCADE, null=True)

class Canvas(models.Model):
    class Meta:
        db_table = 'canvas'
    id = models.IntegerField(primary_key=True)
    user_id = models.ForeignKey(User, models.CASCADE)
    name = models.CharField(unique=True, max_length=100)
    HeightsMap = models.TextField()
    TriPolArray = models.TextField()
    Color = models.TextField()
    
class Params(models.Model):
    class Meta:
        db_table = 'params'
    canvas_id = models.OneToOneField(Canvas, models.CASCADE)
    width = models.IntegerField()
    height = models.IntegerField()
    range = models.FloatField()
    smooth = models.BooleanField()
    mult = models.IntegerField()
    red = models.IntegerField()
    green = models.IntegerField()
    blue = models.IntegerField()
    size = models.IntegerField()