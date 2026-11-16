% Caracteristica estática:
% Zona muerta
Zn=6;
Zp=-6;

u=[-255 -200 -150 -100 -50 Zn 0 Zp 50 100 150 200 255];
dy=[(-2226.6-2296.1)/2  %-255
    (-1729.2-1782.0)/2  %-200
    (-1285.4-1338.1)/2  %-150
    (-854.7-885.5)/2    %-100
    (-404.3-421.9)/2    %-50
    0                   %Zn
    (0)/2               %0
    0                   %Zp
    (399.9+421.9)/2     %50
    (835+861.3)/2       %100
    (1272.2+1320.6)/2   %150
    (1718.3+1788.6)/2   %200
    (2199.5+2298.3)/2]; %255

%% Dibujar la característica estática
[uPlot, orden] = sort(u(:));
dyPlot = dy(:);
dyPlot = dyPlot(orden);

limites = sort([Zn Zp]);
margen = 0.08 * (max(dyPlot) - min(dyPlot));
yLim = [min(dyPlot)-margen, max(dyPlot)+margen];
azul = [0.00 0.27 0.51];

figure('Color', 'w', 'Position', [100 100 900 500]);
ax = axes;
hold(ax, 'on');

zona = patch(ax, limites([1 2 2 1]), yLim([1 1 2 2]), ...
    [0.85 0.38 0.16], 'FaceAlpha', 0.16, 'EdgeColor', 'none');

yline(ax, 0, '-', 'Color', [0.55 0.55 0.55]);
curva = plot(ax, uPlot, dyPlot, 'o-', ...
    'Color', azul, 'LineWidth', 1.8, ...
    'MarkerFaceColor', azul, 'MarkerSize', 5);

title(ax, 'Característica estática del motor CC');
xlabel(ax, 'PWM, u');
ylabel(ax, 'Velocidad en régimen permanente, dy (°/s)');
legend(ax, [curva zona], {'Medidas', 'Zona muerta'}, ...
    'Location', 'northwest', 'Box', 'off');

xlim(ax, [-270 270]);
ylim(ax, yLim);
xticks(ax, [-255 -200 -150 -100 -50 0 50 100 150 200 255]);
grid(ax, 'on');
set(ax, 'FontSize', 12, 'Layer', 'top', 'GridAlpha', 0.15);

colorZM = [0.65 0.25 0.10];

xline(ax, limites(1), '--', 'Color', colorZM, 'HandleVisibility', 'off');
xline(ax, limites(2), '--', 'Color', colorZM, 'HandleVisibility', 'off');

text(ax, limites(1), 0, sprintf('Zn', limites(1)), ...
    'HorizontalAlignment', 'right', 'VerticalAlignment', 'top', ...
    'Color', colorZM, 'FontWeight', 'bold');

text(ax, limites(2), 0, sprintf('Zp', limites(2)), ...
    'HorizontalAlignment', 'left', 'VerticalAlignment', 'top', ...
    'Color', colorZM, 'FontWeight', 'bold');