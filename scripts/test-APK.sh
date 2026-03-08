emul=$1 # Path to emulator
apk=$2  # Path to APK

echo ">>>>> (Start) Test $2 with $1 <<<<<"

if [ -z "$emul" ] || [ -z "$apk" ]; then
  echo "error: usage: $0 <emulator-serial> <apk-path>"
  exit 255
fi

if [ ! -f "$apk" ]; then
  echo "error: APK does not exist: $apk"
  exit 255
fi

# Check if failed
if unzip -t $apk >/dev/null; then
    echo ">> (Info) ZIP OK"
    if apkanalyzer apk summary $apk >/dev/null; then
      echo ">> (Info) APK Summary is Valid"
      python3 $emul/start.py -a $apk -d emulator-5554 -policy bfs_greedy -count 100 -interval 2 -timeout 20 -o results/fdroid_run -is_emulator -grant_perm > $apk.results 2>&1
      if [ $? -eq 0 ]; then
        echo ">> (Info) emulation is OK"
        grep "NFO:Device:App installed: org.fdroid.fdroid" $apk.results
        grep "Please wait while installing the app..." $apk.results | wc -l
      else
        echo ">> (Info) emulation is BROKEN"
        exit 255
      fi
    else
      echo ">> (Info) APK Summary is BROKE"
      exit 255
    fi
else
  echo ">> (Info) ZIP BROKEN"
  exit 255
fi
echo ">>>>>>>>>>>>>>>>>>>>"
exit 0
