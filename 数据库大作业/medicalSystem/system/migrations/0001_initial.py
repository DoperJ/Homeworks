# -*- coding: utf-8 -*-
# Generated by Django 1.11.3 on 2017-12-30 02:34
from __future__ import unicode_literals

from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
    ]

    operations = [
        migrations.CreateModel(
            name='BlogPost',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('title', models.CharField(max_length=150)),
                ('body', models.TextField()),
                ('date', models.DateTimeField()),
            ],
        ),
        migrations.CreateModel(
            name='bookingForm',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('booking_time', models.DateTimeField()),
            ],
        ),
        migrations.CreateModel(
            name='Department',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('name', models.CharField(max_length=20)),
                ('address', models.CharField(max_length=100)),
                ('phone', models.CharField(max_length=20)),
                ('head', models.CharField(max_length=50)),
            ],
        ),
        migrations.CreateModel(
            name='Diagnosis',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('diagnosis_date', models.DateField()),
                ('syptom', models.TextField()),
            ],
        ),
        migrations.CreateModel(
            name='Doctor',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('name', models.CharField(max_length=50)),
                ('gender', models.CharField(choices=[('M', 'Male'), ('F', 'Female')], max_length=2)),
                ('birthdate', models.DateField(blank=True, null=True)),
                ('socialID', models.PositiveIntegerField(null=True)),
                ('phone_number', models.CharField(max_length=11)),
                ('education', models.CharField(max_length=50)),
                ('salary', models.PositiveIntegerField(null=True)),
                ('inductin_date', models.DateField(null=True)),
                ('department', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='staff', to='system.Department')),
            ],
        ),
        migrations.CreateModel(
            name='MedicalRecordBook',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('acquired_date', models.DateField()),
            ],
        ),
        migrations.CreateModel(
            name='Medicine',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('name', models.CharField(max_length=100)),
                ('price', models.PositiveIntegerField()),
                ('production_date', models.DateField()),
                ('usage', models.TextField()),
                ('shelf_life', models.CharField(max_length=20)),
            ],
        ),
        migrations.CreateModel(
            name='Prescription',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('medicine_number', models.PositiveIntegerField()),
                ('diagnosis', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='prescription', to='system.Diagnosis')),
                ('medicine', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='system.Medicine')),
            ],
        ),
        migrations.CreateModel(
            name='Profile',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('name', models.CharField(max_length=50)),
                ('gender', models.CharField(choices=[('M', 'Male'), ('F', 'Female')], max_length=2)),
                ('birthdate', models.DateField(blank=True, null=True)),
                ('socialID', models.CharField(max_length=20)),
                ('phone_number', models.CharField(max_length=11)),
                ('department', models.ManyToManyField(related_name='patient', through='system.bookingForm', to='system.Department')),
                ('doctor', models.ManyToManyField(related_name='patient', through='system.Diagnosis', to='system.Doctor')),
                ('medical_record', models.OneToOneField(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, to='system.MedicalRecordBook')),
                ('user', models.OneToOneField(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL)),
            ],
        ),
        migrations.AddField(
            model_name='diagnosis',
            name='doctor',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='diagnosis', to='system.Doctor'),
        ),
        migrations.AddField(
            model_name='diagnosis',
            name='medicine',
            field=models.ManyToManyField(through='system.Prescription', to='system.Medicine'),
        ),
        migrations.AddField(
            model_name='diagnosis',
            name='patient',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='Patient', to='system.Profile'),
        ),
        migrations.AddField(
            model_name='diagnosis',
            name='record_book',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, related_name='diagnosis', to='system.MedicalRecordBook'),
        ),
        migrations.AddField(
            model_name='bookingform',
            name='department',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='system.Department'),
        ),
        migrations.AddField(
            model_name='bookingform',
            name='patient',
            field=models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='system.Profile'),
        ),
    ]
