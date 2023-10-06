LOGDATE=`date "+%Y%m%d%H%M%S"`
DIRN="/home/user/software/test-20230227/rpcomfit/"
while `sleep 1`;
do
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 ARST >> $DIRN/log_el_$LOGDATE.txt;
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 SVON >> $DIRN/log_el_$LOGDATE.txt;
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 VF=1 >> $DIRN/log_el_$LOGDATE.txt;
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 P=-1000000 >> $DIRN/log_el_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 P=600000 >> $DIRN/log_el_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 P=-500000 >> $DIRN/log_el_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 P=300000 >> $DIRN/log_el_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 P=-200000 >> $DIRN/log_el_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 P=100000 >> $DIRN/log_el_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 P=-100000 >> $DIRN/log_el_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 P=10000 >> $DIRN/log_el_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 P=-10000 >> $DIRN/log_el_$LOGDATE.txt;
    sleep 60
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
    date "+%Y%m%d-%H%M%S" >> $DIRN/log_el_$LOGDATE.txt;
    /home/user/software/test-20230227/rpcomfit/run_ha770 2 PMON >> $DIRN/log_el_$LOGDATE.txt;
    sleep 3
done


