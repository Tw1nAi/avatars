Musisz mieć działający docker-engine w tle (np uruchomiony docker desktop).- wypakuj zahaslowane.zip (hasło Awatary123)
- wypakuj *_awatary.packed.tar.gz (tar -xzvf <file>). Powinny być tam: docker-compose.img.prod.yml, pg_data, dists
- przejdź do folderu dists, tam powinien być plik *awatary_imgs.tar w którym są kontenery
- wczytaj kontenery: docker load -i <..._awatary_imgs.tar>
Jak się wczytają to wróć do folderu z docker compose i danymi bazy danych (pg_data)
- docker compose -f docker-compose.img.prod.yml up -d
testowo można wejść w przeglądarce na adres: http://localhost:5001/