kubectl delete -f influxdb-service.yaml
kubectl apply -f influxdb-service.yaml
kubectl delete -f influxdb-deployment.yaml
kubectl apply -f influxdb-deployment.yaml
./nginx-redeploy.sh
