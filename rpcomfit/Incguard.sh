
for file in `\find . -maxdepth 1 \( -name '*.c' -o -name '*.cc' -o -name '*.cxx' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp'   \)`
do
    if [ `cat ${file} | grep -c "INCLUDE_GUARD_UUID"`  -ge "2" ] ; then
        # echo "${file} already has include guard."
        continue
    fi
    echo "GUARD "${file}
    cp ${file} ${file}.bkup
    UUID=`uuidgen`
    echo "#ifndef INCLUDE_GUARD_UUID_${UUID//-/_}" >  ${file}
    echo "#define INCLUDE_GUARD_UUID_${UUID//-/_}" >> ${file}
    cat ${file}.bkup >> ${file}
    echo "#endif" >> ${file}
    echo "" >> ${file}
    rm ${file}.bkup
done

