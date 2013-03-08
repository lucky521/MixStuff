from django.http import HttpResponse

from django.template import Template, Context

def index(request):
	return HttpResponse("Welcome!")


def hello(request):
	return HttpResponse("Hello world.")


import datetime
def current_datetime(request, param1):
	fp = open('mytemplate.html')
	t = Template( fp.read() )
	fp.close()
	
	now = datetime.datetime.now()
	c = Context({'current_date': now,'your_name': param1})
	html = t.render(c)
	return HttpResponse(html)



