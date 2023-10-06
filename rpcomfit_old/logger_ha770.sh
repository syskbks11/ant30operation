LOGDATE=`date "+%Y%m%d%H%M%S"`
DIRN="/home/user/software/test-20230227/rpcomfit/"
while `sleep 1`;
do
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 ARST >> $DIRN/log_az_$LOGDATE.txt;
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 SVON >> $DIRN/log_az_$LOGDATE.txt;
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 VF=1 >> $DIRN/log_az_$LOGDATE.txt;
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 P=1000000 >> $DIRN/log_az_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 P=-600000 >> $DIRN/log_az_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 P=500000 >> $DIRN/log_az_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 P=-300000 >> $DIRN/log_az_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 P=200000 >> $DIRN/log_az_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 P=-100000 >> $DIRN/log_az_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 P=100000 >> $DIRN/log_az_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 P=-10000 >> $DIRN/log_az_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 P=10000 >> $DIRN/log_az_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_az_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 1 PMON >> $DIRN/log_az_$LOGDATE.txt;
    sleep 3
done


