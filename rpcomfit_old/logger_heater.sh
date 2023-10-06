LOGDATE=`date "+%Y%m%d%H%M%S"`
MYDIR="/home/user/software/test-20230227/rpcomfit/"
while `sleep 10`;
do
echo `date "+%Y%m%d-%H%M%S"`"  "`sleep 0.5; $MYDIR/run_heater 3 1 PV1`,`sleep 0.5; $MYDIR/run_heater 3 2 PV1`,`sleep 0.5; $MYDIR/run_heater 3 3 PV1` >> $MYDIR/log_heater_$LOGDATE.txt;
done


