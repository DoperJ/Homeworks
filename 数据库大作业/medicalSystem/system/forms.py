from django import forms
from .models import Profile, bookingForm

class LoginForm(forms.Form):
    InputUsername = forms.CharField(max_length=20)
    InputPassword = forms.CharField(max_length=20)


class RegisterForm(forms.Form):
    InputUsername = forms.CharField(max_length=20)
    InputEmail = forms.EmailField()
    InputPassword1 = forms.CharField(max_length=20)
    InputPassword2 = forms.CharField(max_length=20)

class ProfileForm(forms.ModelForm):
    class Meta:
        model = Profile
        fields = ('name', 'gender', 'birthdate', 'socialID', 'phone_number', 'medical_record')
        
        
class BookForm(forms.Form):
    Department = forms.CharField(max_length=20)