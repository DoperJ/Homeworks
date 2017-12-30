#from django.db import models
from django.db.models import Model
from django.db.models import ForeignKey, OneToOneField, ManyToManyField
from django.db.models import PositiveIntegerField, DateField, CharField
from django.db.models import DateTimeField, TextField
from django.contrib.auth.models import User
from django.dispatch import receiver
from django.db.models.signals import post_save

class Department(Model):
    name = CharField(max_length=20)
    address = CharField(max_length=100)
    phone = CharField(max_length=20)
    head = CharField(max_length=50)
    def __str__(self):
        return self.name

class Doctor(Model):
    GENDER_CHOICES = ((u'M', u"Male"),
                      (u'F', u"Female"),)
    name = CharField(max_length=50)
    gender = CharField(max_length=2, choices=GENDER_CHOICES)
    birthdate = DateField(null=True, blank=True)
    socialID = PositiveIntegerField(null=True)
    phone_number = CharField(max_length=11)
    education = CharField(max_length=50)
    salary = PositiveIntegerField(null=True)
    inductin_date = DateField(null=True)
    department = ForeignKey(Department, related_name="staff")
    def __str__(self):
        return self.name


class MedicalRecordBook(Model):
    acquired_date = DateField()
    def __str__(self):
        return self.profile.name


class Medicine(Model):
    name = CharField(max_length=100)
    price = PositiveIntegerField()
    production_date = DateField()
    usage = TextField()
    shelf_life = CharField(max_length=20)
    def __str__(self):
        return self.name


class Profile(Model):
    GENDER_CHOICES = ((u'M', u"Male"),
                      (u'F', u"Female"),)
    user = OneToOneField(User, null=True, blank=True)
    name = CharField(max_length=50)
    gender = CharField(max_length=2, choices=GENDER_CHOICES)
    birthdate = DateField(blank=True, null=True)
    socialID = CharField(max_length=20)
    phone_number = CharField(max_length=11)
    medical_record = OneToOneField(MedicalRecordBook, null=True, blank=True)
    doctor = ManyToManyField(Doctor, through="Diagnosis", related_name="patient")
    department = ManyToManyField(Department, through="bookingForm", related_name="patient")
    def __str__(self):
        return self.user.username
    
@receiver(post_save, sender=User)
def create_user_profile(sender, instance, created, **kwargs):
    if created:
        Profile.objects.create(user=instance)
        
@receiver(post_save, sender=User)
def save_user_profile(sender, instance, **kwargs):
    instance.profile.save()
    
    
class bookingForm(Model):
    patient = ForeignKey(Profile)
    department = ForeignKey(Department)
    booking_time = DateTimeField()
    def __str__(self):
        return self.booking_time.ctime()
    
    
class Diagnosis(Model):
    doctor = ForeignKey(Doctor, related_name="diagnosis")
    patient = ForeignKey(Profile, related_name="Patient")
    record_book = ForeignKey(MedicalRecordBook, related_name="diagnosis")
    medicine = ManyToManyField(Medicine, through="Prescription")
    diagnosis_date = DateField()
    syptom = TextField()
    def __str__(self):
        return self.syptom


class Prescription(Model):
    diagnosis = ForeignKey(Diagnosis, related_name="prescription")
    medicine = ForeignKey(Medicine)
    medicine_number = PositiveIntegerField()
    def __str__(self):
        return self.medicine_number 
    
class BlogPost(Model):
    title = CharField(max_length=150)
    body = TextField()
    date = DateTimeField()
    def __str__(self):
        return self.title
