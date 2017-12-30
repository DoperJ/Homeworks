from django.contrib import admin
from system.models import BlogPost
from django.contrib.auth.models import User
from .models import Profile, Medicine, Department, Doctor, bookingForm
from .models import Diagnosis, MedicalRecordBook


class ProfileAdmin(admin.ModelAdmin):
    fields = ('user', 'name', 'gender', 'birthdate', 'socialID', 'phone_number',)
    
   
admin.site.register(BlogPost)
admin.site.register(Profile, ProfileAdmin)
admin.site.register(Medicine)
admin.site.register(Doctor)
admin.site.register(Department)
admin.site.register(bookingForm)
admin.site.register(Diagnosis)
admin.site.register(MedicalRecordBook)