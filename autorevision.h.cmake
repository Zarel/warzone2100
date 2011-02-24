#ifndef AUTOREVISION_H
#define AUTOREVISION_H

#ifndef SVN_AUTOREVISION_STATIC
#define SVN_AUTOREVISION_STATIC static
#endif

#define SVN_LOW_REV             ${SVN_REV}
#define SVN_LOW_REV_STR         "${SVN_REV_STR}"
#define SVN_REV                 ${SVN_REV}
#define SVN_REV_STR             "${SVN_REV_STR}"
#define SVN_DATE                "${SVN_DATE}"
#define SVN_URI                 "${SVN_URI}"
#define SVN_TAG                 "${SVN_TAG}"

#define SVN_SHORT_HASH                  "${SVN_SHORT_HASH}"
#define SCN_SHORT_HASH_WITHOUT_QUOTES   ${SVN_SHORT_HASH}

#define SVN_WC_MODIFIED ${WC_MODIFIED}
#define SVN_WC_SWITCHED 0

SVN_AUTOREVISION_STATIC const char svn_low_revision_cstr[] = "${SVN_REV_STR}";
SVN_AUTOREVISION_STATIC const char svn_revison_cstr[] =      "${SVN_REV_STR}";
SVN_AUTOREVISION_STATIC const char svn_date_cstr[] =         "${SVN_DATE}";
SVN_AUTOREVISION_STATIC const char svn_uri_cstr[] =          "${SVN_URI}";

#endif
