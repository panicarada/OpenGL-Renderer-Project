Data = load('Poisson Map.txt');
X = Data(1, 1 : 3 : end);
Y = Data(1, 2 : 3 : end);
Z = Data(1, 3 : 3 : end);

scatter3(X, Y, Z);
saveas(gcf, ['Poisson Map ' num2str(size(X, 2)) '.jpg']);