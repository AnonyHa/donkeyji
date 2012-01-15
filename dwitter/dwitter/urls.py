from django.conf.urls.defaults import *

# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    # Example:
    # (r'^dwitter/', include('dwitter.foo.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    # (r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    (r'^admin/', include(admin.site.urls)),

	(r'^$', 'dwitter.account.views.handle_root'),
	(r'^home/$', 'dwitter.account.views.handle_home'),
	(r'^login/$', 'dwitter.account.views.handle_login'),
	(r'^signup/$', 'dwitter.account.views.handle_signup'),
	(r'^signin/$', 'dwitter.account.views.handle_signin'),
	(r'^logout/$', 'dwitter.account.views.handle_logout'),
)
