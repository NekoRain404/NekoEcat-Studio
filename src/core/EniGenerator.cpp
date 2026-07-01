// EniGenerator — generates EtherCAT Network Information (ENI) XML.

#include "EniGenerator.h"

#include <QTextStream>

void EniGenerator::addSlave(const EniSlaveConfig &slave)
{
    slaves_.append(slave);
}

void EniGenerator::clear()
{
    slaves_.clear();
}

QString EniGenerator::xmlEscape(const QString &text)
{
    QString out = text;
    out.replace('&', "&amp;");
    out.replace('<', "&lt;");
    out.replace('>', "&gt;");
    out.replace('"', "&quot;");
    out.replace('\'', "&apos;");
    return out;
}

QString EniGenerator::buildMailbox(const EniSlaveConfig &slave) const
{
    if (!slave.supportsCoE && !slave.supportsEoE &&
        !slave.supportsFoE && !slave.supportsSoE) {
        return {};
    }

    QString out;
    QTextStream s(&out);
    s << "      <Mailbox>\n";
    if (slave.supportsCoE) s << "        <CoE/>\n";
    if (slave.supportsEoE) s << "        <EoE/>\n";
    if (slave.supportsFoE) s << "        <FoE/>\n";
    if (slave.supportsSoE) s << "        <SoE/>\n";
    s << "      </Mailbox>\n";
    return out;
}

QString EniGenerator::buildProcessData(const EniSlaveConfig &slave) const
{
    if (slave.pdos.isEmpty()) {
        return {};
    }

    QString out;
    QTextStream s(&out);
    s << "      <ProcessData>\n";

    for (const auto &pdo : slave.pdos) {
        const char *tag = pdo.isTxPdo ? "TxPdo" : "RxPdo";
        s << "        <" << tag << " Fixed=\"1\" Mandatory=\"0\">\n";
        s << "          <Index>#x" << QString::number(pdo.index, 16).rightJustified(4, '0')
          << "</Index>\n";
        s << "          <Name>" << xmlEscape(pdo.name) << "</Name>\n";
        for (const auto &e : pdo.entries) {
            s << "          <Entry>\n";
            s << "            <Index>#x" << QString::number(e.index, 16).rightJustified(4, '0')
              << "</Index>\n";
            s << "            <SubIndex>" << e.subIndex << "</SubIndex>\n";
            s << "            <BitLen>" << e.bitLength << "</BitLen>\n";
            if (!e.name.isEmpty()) {
                s << "            <Name>" << xmlEscape(e.name) << "</Name>\n";
            }
            s << "          </Entry>\n";
        }
        s << "        </" << tag << ">\n";
    }

    s << "      </ProcessData>\n";
    return out;
}

QString EniGenerator::buildInitCmds(const EniSlaveConfig &slave) const
{
    if (slave.startupCmds.isEmpty()) {
        return {};
    }

    QString out;
    QTextStream s(&out);
    s << "      <InitCmds>\n";
    for (const auto &cmd : slave.startupCmds) {
        s << "        <InitCmd>\n";
        s << "          <Transition>" << xmlEscape(cmd.transition) << "</Transition>\n";
        s << "          <Index>#x" << QString::number(cmd.index, 16).rightJustified(4, '0')
          << "</Index>\n";
        s << "          <SubIndex>" << cmd.subIndex << "</SubIndex>\n";
        s << "          <Data>" << xmlEscape(cmd.dataHex) << "</Data>\n";
        if (!cmd.comment.isEmpty()) {
            s << "          <Comment>" << xmlEscape(cmd.comment) << "</Comment>\n";
        }
        s << "        </InitCmd>\n";
    }
    s << "      </InitCmds>\n";
    return out;
}

QString EniGenerator::buildSlaveElement(const EniSlaveConfig &slave) const
{
    QString out;
    QTextStream s(&out);
    s << "    <Slave>\n";
    s << "      <Info>\n";
    s << "        <Name>" << xmlEscape(slave.name) << "</Name>\n";
    s << "        <PhysAddr>" << (1001 + slave.position) << "</PhysAddr>\n";
    s << "        <AutoIncAddr>" << (-slave.position & 0xFFFF) << "</AutoIncAddr>\n";
    if (slave.alias != 0) {
        s << "        <Alias>" << slave.alias << "</Alias>\n";
    }
    s << "        <VendorId>" << slave.vendorId << "</VendorId>\n";
    s << "        <ProductCode>" << slave.productCode << "</ProductCode>\n";
    s << "        <RevisionNo>" << slave.revisionNo << "</RevisionNo>\n";
    s << "      </Info>\n";

    s << buildMailbox(slave);
    s << buildProcessData(slave);
    s << buildInitCmds(slave);

    s << "    </Slave>\n";
    return out;
}

QString EniGenerator::generate() const
{
    QString out;
    QTextStream s(&out);

    s << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    s << "<EtherCATConfig xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
         "xmlns=\"http://www.ethercat.org/2007/EtherCATInfo\" Version=\"1.0\">\n";

    // Config / Master section.
    s << "  <Config>\n";
    s << "    <Master>\n";
    s << "      <Info>\n";
    s << "        <Name>" << xmlEscape(masterName_) << "</Name>\n";
    s << "        <Source>NekoEcat Studio</Source>\n";
    s << "      </Info>\n";
    s << "      <CyclicFrames>\n";
    s << "        <Frame>\n";
    s << "          <Cmd>\n";
    s << "            <Cycle>" << cycleTimeUs_ << "</Cycle>\n";
    s << "          </Cmd>\n";
    s << "        </Frame>\n";
    s << "      </CyclicFrames>\n";
    s << "    </Master>\n";

    // Slaves.
    for (const auto &slave : slaves_) {
        s << buildSlaveElement(slave);
    }

    s << "  </Config>\n";
    s << "</EtherCATConfig>\n";

    return out;
}
