LOGDATE=`date "+%Y%m%d%H%M%S"`
while `sleep 10`;
do
	echo `date "+%Y%m%d-%H%M%S"`"  "`/home/user/software/test-20230227/rpcomfit/run_lakeshore 4 KRDG?` >>/home/user/software/test-20230227/rpcomfit/log_lakeshore_$LOGDATE.txt;
done


