Data = load('Poisson Map.txt');
X = Data(1, 1 : 2 : end);
Y = Data(1, 2 : 2 : end);
plot(X, Y, '.');
saveas(gcf, ['Poisson Map ' num2str(size(X, 2)) '.jpg']);