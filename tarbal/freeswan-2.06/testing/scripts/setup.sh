
if [ -z "$FREESWANSRCDIR" ]
then
	if [ -f ../../umlsetup.sh ]
	then
	  FREESWANSRCDIR=`cd ../.. && pwd`
	else 
	  if [ -f ../../../umlsetup.sh ]
	  then 
	    FREESWANSRCDIR=`cd ../../.. && pwd`
	  fi
        fi  	
fi

if [ ! -f $FREESWANSRCDIR/umlsetup.sh ]
then
	echo Umlsetup not found at FREESWANSRCDIR=$FREESWANSRCDIR.
	echo Is FREESWANSRCDIR set correctly'?'
	exit 5
fi

FREESWANSRCDIR=`cd ${FREESWANSRCDIR}; pwd`
export FREESWANSRCDIR

TESTINGROOT=${FREESWANSRCDIR}/testing
UTILS=`cd ${TESTINGROOT}/utils && pwd`
FIXUPDIR=`cd ${FREESWANSRCDIR}/testing/klips/fixups && pwd`
NJ=${UTILS}/uml_netjig/uml_netjig

REGRESSRESULTS=${REGRESSRESULTS-results}



