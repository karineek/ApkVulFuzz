DROIDBOT_DIR=$1  ## e.g. "/users/kevenmen/droidbot/"
SEEDS_DIR=$2     ## e.g."/users/kevenmen/ApkVulFuzz/Evaluation-SSBSE-2026/seeds"

for apk in "$SEEDS_DIR"/*.apk; do
  echo "=== Testing: $apk ==="
  ./test-APK.sh "$DROIDBOT_DIR" "$apk"
  echo
done
