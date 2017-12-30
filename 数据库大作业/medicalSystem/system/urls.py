from django.conf.urls import url, include
from django.views.generic import ListView, DetailView
from .views import home, index, log_in, log_out, register, profile, book
from system.models import BlogPost, Profile
from django.contrib import admin
admin.autodiscover()

urlpatterns = [
    url(r"^$", home, name="home"),
    url(r"^home$", home, name="home"),
    url(r"^home/(?P<pk>\d+)$", DetailView.as_view(
                        model = BlogPost,
                        template_name = "system/post.html")),
    url(r'^login$', log_in, name='login'),
    url(r'^logout$', log_out, name='logout'),
    url(r'^register$', register, name='register'),
    url(r'^profile$', profile, name='profile'),
    url(r'^book$', book, name='book'),
    url(r'^index$', index, name='index'),
    url(r"^accounts/login", log_in, name="login"),
]
