#ifndef QTTXTDATALOADDLG_H
#define QTTXTDATALOADDLG_H

#include <QDialog>
#include <memory>
#include <vector>

namespace Ui {
class txt_data_load_dlg;
}

class KvData;
class KgTxtDataLoader;
class QLineEdit;

class QtTxtDataLoadDlg : public QDialog
{
    Q_OBJECT

public:
    explicit QtTxtDataLoadDlg(QString path, QWidget *parent = nullptr);
    ~QtTxtDataLoadDlg();

    void accept() override;

    std::shared_ptr<KvData> data;

private:
    void reload();

    void updateImportAs_();

    static void updateIntEdit_(QLineEdit* edit, int low, int high);

    static std::shared_ptr<KvData> makeSeries_(const std::vector<std::vector<double>>& mat);
    static std::shared_ptr<KvData> makeMatrix_(const std::vector<std::vector<double>>& mat);
    static std::shared_ptr<KvData> makeSampled1d_(const std::vector<std::vector<double>>& mat);
    static std::shared_ptr<KvData> makeSampled2d_(const std::vector<std::vector<double>>& mat);
    static std::shared_ptr<KvData> makeScattered_(const std::vector<std::vector<double>>& mat, unsigned dim);

private:
    Ui::txt_data_load_dlg *ui;
    QString path_;
    std::unique_ptr<KgTxtDataLoader> loader_;
    std::vector<std::vector<double>> mat_;
};

#endif // QTTXTDATALOADDLG_H
