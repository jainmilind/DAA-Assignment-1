g++ main.cpp -o main.out -g
./main.out < testcases/$1.txt > out.txt

cd Visualizer
g++ visualiser.cpp -o visualiser.out
./visualiser.out < .././out.txt


rm visualiser.out
cd ..
rm main.out

