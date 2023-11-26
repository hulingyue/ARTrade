ids=$(ipcs -m | awk '{print $2}' | grep -E "[0-9]+")
for id in $ids
do
    $(ipcrm -m $id)
done