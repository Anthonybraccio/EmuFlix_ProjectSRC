/**
 * @file EmulatorInstaller.cpp
 * @brief Implements installation for bundled emulator packages.
 */
#include "library/EmulatorInstaller.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QString>

/**
 * @brief Installs the emulator package for the requested system.
 *
 * @param system Console system to install.
 * @param destination Folder to extract the emulator into.
 * @return Install result with success status and a message.
 */
InstallResult EmulatorInstaller::installSystem(const std::string& system, const std::string& destination) const {
    InstallResult result;
    result.system = system;

    QString packagePath = getPackagePath(system);
    if(packagePath.isEmpty() == true) {
        result.message = "Unsupported system selected for installation.";
        return result;
    }

    QString installedSystemPath = getInstalledSystemPath(system);
    if(installedSystemPath.isEmpty() == true) {
        result.message = "No installed exe path is configured for this system.";
        return result;
    }

    QString destError;
    QString destPath = QString::fromStdString(destination).trimmed();
    if(checkDestination(destPath, destError) == false) {
        result.message = destError.toStdString();
        return result;
    }

    QString extractionError;
    if(extractZip(packagePath, destPath, extractionError) == false) {
        result.message = extractionError.toStdString();
        return result;
    }
    
    QString finalExePath = QDir(destPath).filePath(installedSystemPath);

    QFileInfo finalExeFile(finalExePath);
    if(finalExeFile.exists() == false || finalExeFile.isFile() == false) {
        result.message = "Installation finished, but the emulator exe cannot be found.";
        return result;
    }

    result.sucess = true;
    result.executablePath = finalExePath.toStdString();
    result.message = "Installed successfully.";
    return result;
}


/**
 * @brief Builds the path to the bundled Windows assets folder.
 *
 * @return Path to the emulator assets folder.
 */
QString EmulatorInstaller::getAssetsPath() const {
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();

    return dir.filePath("assets/windows");
}


/**
 * @brief Finds the zip file used to install a system's emulator.
 *
 * @param system Console system name.
 * @return Zip package path, or empty string when unsupported.
 */
QString EmulatorInstaller::getPackagePath(const std::string& system) const {
    QDir assetsFolder(getAssetsPath());

    if(isMesen(system) == true) {
        return assetsFolder.filePath("mesen/mesen.zip");
    }

    if(isGopher(system) == true) {
        return assetsFolder.filePath("gopher64/gopher64.zip");
    }

    return QString();
}


/**
 * @brief Finds the expected emulator exe after the zip is extracted.
 *
 * @param system Console system name.
 * @return Relative exe path for that emulator.
 */
QString EmulatorInstaller::getInstalledSystemPath(const std::string& system) const {
    if(isMesen(system) == true) {
        return QString("mesen/Mesen.exe");
    }

    if(isGopher(system) == true) {
        return QString("gopher64/gopher64-windows-x86_64.exe");
    }

    return QString();
}


/**
 * @brief Checks whether a system should use the Mesen package.
 *
 * @param system Console system name.
 * @return `true` when Mesen supports this system.
 */
bool EmulatorInstaller::isMesen(const std::string& system) const {
    return system == "NES" ||
           system == "Super NES" ||
           system == "Game Boy Advance" ||
           system == "Game Boy" ||
           system == "Game Boy Color";
}


/**
 * @brief Checks whether a system should use the gopher64 package.
 *
 * @param system Console system name.
 * @return `true` for Nintendo 64.
 */
bool EmulatorInstaller::isGopher(const std::string& system) const {
    return system == "Nintendo 64";
}


/**
 * @brief Makes sure the destination folder is ready for install.
 *
 * @param destination Folder path chosen by the user.
 * @param errorMessage Output message when the folder is invalid.
 * @return `true` if the folder exists or was created.
 */
bool EmulatorInstaller::checkDestination(const QString& destination, QString& errorMessage) const {
    QString path = destination.trimmed();
    if(path.isEmpty() == true) {
        errorMessage = "Install folder was empty.";
        return false;
    }

    QDir dir(path);
    if (dir.exists() == true) {
        return true;
    }

    if(dir.mkpath(".") == false) {
        errorMessage = "Inputed install folder could not be created.";
        return false;
    }

    return true;
}


/**
 * @brief Extracts a zip package using PowerShell.
 *
 * @param zipPath Path to the zip package.
 * @param destination Folder where the package should be extracted.
 * @param errorMessage Output message when extraction fails.
 * @return `true` if extraction completed successfully.
 */
bool EmulatorInstaller::extractZip(const QString& zipPath, const QString& destination, QString& errorMessage) const {
    errorMessage.clear();
    
    QFileInfo zipFile(zipPath);
    if(zipFile.exists() == false || zipFile.isFile() == false) {
        errorMessage = "Emulator package zip was not found.";
        return false;
    }

    QString path = destination.trimmed();
    if(path.isEmpty() == true) {
        errorMessage = "Install folder was empty.";
        return false;
    }

    QString unzipCmd = QString(
        "$ErrorActionPreference = 'Stop'; "
        "Expand-Archive -LiteralPath '%1' -DestinationPath '%2' -Force"
    ).arg(zipPath, path);

    QProcess process;
    process.start(
        "powershell",
        {
            "-NoProfile", "-ExecutionPolicy", "Bypass", "-Command", unzipCmd
        }
    );

    if(process.waitForStarted() == false) {
        errorMessage = "PowerShell could not be started for package extraction.";
        return false;
    }

    if(process.waitForFinished(-1) == false) {
        process.kill();
        errorMessage = "Install failed. Package extraction timed out.";
        return false;
    }

     if(process.exitStatus() != QProcess::NormalExit) {
        errorMessage = "Install failed. Extraction process crashed.";
        return false;
    }

    QString stdError = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
    QString stdOutput = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();

    QString combinedMessage = stdOutput + "\n" + stdError;
    QString loweredMessage = combinedMessage.toLower();
    
    if(process.exitCode() != 0) {
        if(loweredMessage.contains("access") == true || 
           loweredMessage.contains("access") == true || 
           loweredMessage.contains("permission") == true || 
           loweredMessage.contains("unauthorized") == true) {
            errorMessage = "Install failed. The selected folder may require admin permissions. Choose another folder or run emuflix as administrator.";
            return false;
        }

        errorMessage = "Install failed. Could not extract emulator package.";
        return false;
    }
    return true;
}      
