% exportar_versiones.m
% Exporta MotorCC_V3.slx a versiones anteriores de MATLAB/Simulink.
% Ejecutar con MATLAB R2025a desde la carpeta software/matlab/
%
% Uso:
%   cd('<ruta>/Motor-CC-Control-Lab/software/matlab')
%   run('exportar_versiones.m')

modelo    = 'MotorCC_V3_R2025b';               % archivo fuente (versión de trabajo)
base      = 'MotorCC_V3';                      % prefijo para los archivos exportados
carpeta   = fileparts(mfilename('fullpath'));   % carpeta del script

versiones = { ...
    'R2024b'; ...
    'R2024a'; ...
    'R2023b'; ...
    'R2023a'; ...
    'R2022b'; ...
    'R2021b'; ...
    'R2020b'; ...
};

% Cargar el modelo sin abrirlo visualmente
load_system(fullfile(carpeta, [modelo '.slx']));

for k = 1:numel(versiones)
    ver  = versiones{k};
    dest = fullfile(carpeta, sprintf('%s_%s.slx', base, ver));
    fprintf('Exportando → %s_%s.slx ... ', base, ver);
    try
        save_system(modelo, dest, 'ExportToVersion', ver);
        fprintf('OK\n');
    catch e
        fprintf('ERROR: %s\n', e.message);
    end
end

close_system(modelo, 0);   % cerrar sin guardar cambios en el original
fprintf('\nListo. Archivos generados en:\n  %s\n', carpeta);
