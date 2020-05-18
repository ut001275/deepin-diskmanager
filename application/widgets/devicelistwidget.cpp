#include "devicelistwidget.h"
#include <DPalette>
#include <QVBoxLayout>
#include <QDebug>
#include "customcontrol/dmdiskinfobox.h"
DeviceListWidget::DeviceListWidget(QWidget *parent): DWidget(parent)
{
    setAutoFillBackground(true);
    auto plt = this->palette();
    plt.setBrush(QPalette::Background, QBrush(Qt::white));
    setPalette(plt);
    setMaximumWidth(360);
    setMinimumWidth(100);
    initUi();
    initConnection();
}

void DeviceListWidget::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);
    m_treeview = new DmTreeview(this);
    layout->addWidget(m_treeview);
}

void DeviceListWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::sigUpdateDeviceInfo, this, &DeviceListWidget::slotUpdateDeviceInfo);
    connect(m_treeview, &DmTreeview::sigCurSelectChanged, DMDbusHandler::instance(), &DMDbusHandler::sigCurSelectChanged);
}


void DeviceListWidget::slotUpdateDeviceInfo()
{
    //ToDo:
//更新DmTreeview
//设置当前选项
    DeviceInfoMap infomap = DMDbusHandler::instance()->probDeviceInfo();

    for (auto it = infomap.begin(); it != infomap.end(); it++) {
        DeviceInfo info = it.value();
//        qDebug() << Utils::sector_to_unit(info.sectors, info.sector_size, SIZE_UNIT::UNIT_GIB);

        double_t sectorall = (info.length * info.sector_size) / 1024.0 / 1024.0 / 1024.0;
        QString s_disksize = QString::number(sectorall, 'f', 2) + "GB";
//        QString s_disksize = QString::number(Utils::sector_to_unit(info.sectors, info.sector_size, SIZE_UNIT::UNIT_GIB)) + "GB";
        DmDiskinfoBox *m_box = new DmDiskinfoBox(0, info.m_path, s_disksize);
        for (auto it = info.partition.begin(); it != info.partition.end(); it++) {
//            qDebug() << Utils::sector_to_unit(it->sector_end - it->sector_end, it->sector_size, SIZE_UNIT::UNIT_GIB);

            QString s_pdisksize = QString::number(Utils::sector_to_unit(it->sector_end - it->sector_start, it->sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GB";
            QString s_partitionpath = it->path;
            double s_used = (it->sectors_used * it->sector_size) / 1024.0 / 1024.0 / 1024.0;
            //            qDebug() << s_used;

//            if (it->partition_number == 4)
//                int a = 0;
//            qDebug() << it->sector_end << it->sector_start << it->sectors_used << it->sectors_unused << it->sectors_unallocated;
            double s_unused = (it->sectors_unused * it->sector_size) / 1024.0 / 1024.0 / 1024.0;
            QString s_unusedstr = QString::number(s_unused, 'f', 2) + "GB";
            QString s_usedstr = QString::number(s_used, 'f', 2) + "GB";
            qDebug() << it->partition_number;
            qDebug() << s_unusedstr << "unused";
            qDebug() << s_usedstr << "used";
            qDebug() << it->fstype;
            FSType fstype1 = (FSType)it->fstype;
            QString s_fstype = Utils::FSTypeToString(fstype1);
            qDebug() << it->mountpoints;
            QString s_mountpoint;
            QString s_mountpoints;
            for (int o = 0; o < it->mountpoints.size(); o++) {
                s_mountpoint = it->mountpoints[o];
                s_mountpoints += it->mountpoints[o];
                qDebug() << it->mountpoints[o];
            }
            qDebug() << s_mountpoints;
            qDebug() << s_fstype;
            qDebug() << it->filesystem_label;
            QString s_filesystem_label = it->filesystem_label;
            //            qDebug() << s_unused;
            DmDiskinfoBox *m_childbox = new DmDiskinfoBox(1, "", "", s_partitionpath, s_pdisksize, s_usedstr, s_unusedstr, it->sectors_unallocated,
                                                          it->sector_start, it->sector_end, s_fstype, s_mountpoints, s_filesystem_label);
//            qDebug() << it->path << it << s_pdisksize;
            m_box->childs.append(m_childbox);
        }

        m_treeview->addTopItem(m_box);
    }
}
