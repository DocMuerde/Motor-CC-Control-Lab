% exportar_versiones.m
% Exporta MotorCC_V3_R2025b.slx a versiones anteriores de MATLAB/Simulink.
% Ejecutar con MATLAB R2025b desde la carpeta software/matlab/
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

origen = fullfile(carpeta, [modelo '.slx']);
yaCargado = bdIsLoaded(modelo);
if yaCargado && (~strcmp(get_param(modelo, 'FileName'), origen) || ...
        strcmp(get_param(modelo, 'Dirty'), 'on'))
    error('MotorCC:ModeloAbierto', ...
        'Cierra el modelo homónimo o guarda los cambios del modelo fuente antes de exportar.');
end
load_system(origen);

fallidas = {};
for k = 1:numel(versiones)
    ver  = versiones{k};
    dest = fullfile(carpeta, sprintf('%s_%s.slx', base, ver));
    fprintf('Exportando → %s_%s.slx ... ', base, ver);
    try
        save_system(modelo, dest, 'ExportToVersion', [ver '_SLX']);
        fprintf('OK\n');
    catch e
        fprintf('ERROR: %s\n', e.message);
        fallidas{end + 1} = ver;
    end
end

if ~yaCargado
    close_system(modelo, 0);
end
if ~isempty(fallidas)
    error('MotorCC:ExportacionIncompleta', ...
        'No se pudieron exportar estas versiones: %s.', strjoin(fallidas, ', '));
end
fprintf('\nListo. Archivos generados en:\n  %s\n', carpeta);
