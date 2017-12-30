from django.http import HttpResponse
from datetime import datetime

def hello(request):
	return HttpResponse('Server responses to you!')

def current_datetime(request):
	now = datetime.now()
	html = "<html><body>It is now %s.</body></html>" % now
	return HttpResponse(html)
