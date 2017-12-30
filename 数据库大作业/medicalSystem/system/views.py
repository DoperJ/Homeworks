from django.shortcuts import render, render_to_response, redirect, get_object_or_404
from .forms import LoginForm, RegisterForm, ProfileForm, BookForm
from django.contrib.auth.decorators import login_required
from django.contrib.auth import authenticate, login, logout
from django.contrib.auth.models import User
from django.contrib import messages
from django.template import RequestContext
from django.http import JsonResponse, HttpResponse
from django.views.decorators.cache import cache_page
from .models import Profile, Department, bookingForm, MedicalRecordBook
from .models import Diagnosis, BlogPost
from datetime import datetime, date


def home(request):
    notice_list = BlogPost.objects.all()
    context = {"notice_list": notice_list,
               "today": date.today(),}
    return render(request, "system/header.html", context)

@login_required
def index(request):
    user = request.user
    profile = Profile.objects.get(user=user)
    loginform = LoginForm()
    department_list = Department.objects.all()
    booking_list = bookingForm.objects.filter(patient=profile)
    diagnosis_list = Diagnosis.objects.filter(patient=profile)
    if request.method == "POST":
        book_form = BookForm(request.POST)
        print(book_form)
        if book_form.is_valid():
            print("valid")
            department_name = book_form.cleaned_data['Department']
            department = Department.objects.get(name=department_name)
            bookingForm.objects.create(patient=user.profile,
                                          department=department,
                                          booking_time=datetime.now())
    context = {'loginform':loginform,
               'profile': profile,
               'edit': False,
               'department_list': department_list,
               'booking_list': booking_list,
               'diagnosis_list': diagnosis_list,
               "today": date.today(),}
    return render(request, 'system/index.html', context)

def log_in(request):
    if request.method == 'GET':
        form = LoginForm()
        return render(request, 'system/login.html', {'form': form})
    if request.method == 'POST':
        form = LoginForm(request.POST)
        if form.is_valid():
            username = form.cleaned_data['InputUsername']
            password = form.cleaned_data['InputPassword']
            print(username)
            user = authenticate(username=username, password=password)
            if user is not None and user.is_active:
                login(request, user)
                return redirect('index')
            else:
                return render(request, 'system/login.html', {'form':form, 'error': "Incorrect username or password. Please re-enter "})
        else:
            return render(request, 'system/login.html', {'form': form})

def profile(request):
    user = request.user
    profile = Profile.objects.get(user=user)
    department_list = Department.objects.all()
    if request.method == "GET":
        profile_form = ProfileForm(instance=user.profile)
        context = {'profile': profile,
                   'profile_form': profile_form,
                   'edit':True,
                   'department_list': department_list,}
        return render(request, "system/index.html", context)
    if request.method == "POST":
        form = ProfileForm(request.POST, instance=request.user.profile)
        if form.is_valid():
            form.save()
            return redirect('index')
        else:
            pass

@login_required
def log_out(request):
    logout(request)
    return redirect('home')
    

@login_required
def book(request):
    user = request.user
    newbook = MedicalRecordBook.objects.create(acquired_date=date.today())
    user.profile.medical_record = newbook
    user.profile.save()
    return redirect("index")
        

def register(request):
    if request.method == 'POST':
        form = RegisterForm(request.POST)
        success = False
        if form.is_valid():
            username = form.cleaned_data['InputUsername']
            email = form.cleaned_data['InputEmail']
            password1 = form.cleaned_data['InputPassword1']
            password2 = form.cleaned_data['InputPassword2']
            if password1 != password2:
                error = "Two passwords don't match."
                return render(request, 'system/register.html', {'error': error, 'success': success})
            filterUsername=User.objects.filter(username__exact=username)
            if filterUsername:
                error = "This username exists."
                return render(request, 'system/register.html', {'error': error, 'success': success})
            filterEmail=User.objects.filter(email__exact=email)
            if filterEmail:
                error = "This e-mail exists."
                return render(request, 'system/register.html', {'error': error, 'success': success})
            user = User.objects.create_user(username, email, password1)
            user.save()
            success = True
            return render(request, 'system/register.html', {'success': success})
        else:
            error = "Bad Email!"
            return render(request, 'system/register.html', {'error': error, 'success': success})
    return render(request, 'system/register.html')
