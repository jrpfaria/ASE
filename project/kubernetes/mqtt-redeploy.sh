kubectl delete -f mqtt-conf.yaml
kubectl apply -f mqtt-conf.yaml
kubectl delete -f mqtt-service.yaml
kubectl apply -f mqtt-service.yaml
kubectl delete -f mqtt-deployment.yaml
kubectl apply -f mqtt-deployment.yaml
./nginx-redeploy.sh
