kubectl delete secret traefik-cert -n ase-p1g4
kubectl create secret tls traefik-cert --key traefik.key --cert traefik.crt -n ase-p1g4
kubectl delete -f traefik-ingress.yaml
kubectl apply -f traefik-ingress.yaml
