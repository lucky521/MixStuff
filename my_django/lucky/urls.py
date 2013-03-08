from django.conf.urls.defaults import *

# Uncomment the next two lines to enable the admin:
# from django.contrib import admin
# admin.autodiscover()

urlpatterns = patterns('',
    # Example:
    # (r'^lucky/', include('lucky.foo.urls')),

    # Uncomment the admin/doc line below and add 'django.contrib.admindocs' 
    # to INSTALLED_APPS to enable admin documentation:
    # (r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    # (r'^admin/', include(admin.site.urls)),
)

from lucky.views import index
from lucky.views import hello
from lucky.views import current_datetime

urlpatterns = patterns('',
	(r'^$', index),
	(r'^hello/$', hello),
	(r'^name/(.+)/$', current_datetime),
)



