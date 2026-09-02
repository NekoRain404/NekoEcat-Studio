#include "EtherCATDocumentationService.h"
#include "EventBus.h"
#include "infra/EcatClient.h"

// EtherCATDocumentationService.cpp — Generates structured documentation for the studio
//
// Implementation notes:
//   - Produces four doc types: API, User, Developer, and System documentation
//   - Each document contains titled sections rendered as Markdown
//   - Full-text search across all generated documents with relevance scoring

EtherCATDocumentationService::EtherCATDocumentationService(EventBus* bus, EcatClient* client, QObject* parent)
    : QObject(parent), bus_(bus), client_(client) {}

Documentation EtherCATDocumentationService::makeDoc(const QString& title, const QString& content,
                                                    const QVector<DocSection>& sections, const QString& author) {
    Documentation d;
    d.title = title;
    d.content = content;
    d.sections = sections;
    d.format = QStringLiteral("markdown");
    d.version = QStringLiteral("1.0");
    d.author = author;
    d.timestamp = QDateTime::currentDateTime();
    emit documentationGenerated(d);
    return d;
}

Documentation EtherCATDocumentationService::generateApiDocumentation() {
    QVector<DocSection> sections;

    DocSection overview;
    overview.title = QStringLiteral("API Overview");
    overview.content = QStringLiteral("NekoEcat Studio provides a comprehensive API for EtherCAT "
                                      "network management, slave configuration, and diagnostics.");
    sections << overview;

    DocSection services;
    services.title = QStringLiteral("Service API");
    services.content = QStringLiteral("Services are registered in the ServiceContainer and provide "
                                      "domain-specific functionality. Each service is a QObject with "
                                      "signals for state changes.");
    sections << services;

    DocSection events;
    events.title = QStringLiteral("Event System");
    events.content = QStringLiteral("The EventBus provides publish/subscribe messaging between "
                                    "services and plugins. Events are dispatched on the main thread.");
    sections << events;

    return makeDoc(QStringLiteral("API Documentation"), QStringLiteral("Complete API reference for NekoEcat Studio"),
                   sections, QStringLiteral("NekoEcat Team"));
}

Documentation EtherCATDocumentationService::generateUserDocumentation() {
    QVector<DocSection> sections;

    DocSection getting;
    getting.title = QStringLiteral("Getting Started");
    getting.content = QStringLiteral("Connect to an EtherCAT master daemon, scan the network, "
                                     "and begin commissioning your slaves.");
    sections << getting;

    DocSection workspaces;
    workspaces.title = QStringLiteral("Workspaces");
    workspaces.content = QStringLiteral("NekoEcat Studio organizes functionality into workspaces: "
                                        "Topology, SDO, Watch, Diagnostics, and more.");
    sections << workspaces;

    DocSection config;
    config.title = QStringLiteral("Configuration");
    config.content = QStringLiteral("Use the Configuration workspace to read/write SDO parameters, "
                                    "manage PDO mappings, and configure slave behavior.");
    sections << config;

    return makeDoc(QStringLiteral("User Documentation"), QStringLiteral("End-user guide for NekoEcat Studio"), sections,
                   QStringLiteral("NekoEcat Team"));
}

Documentation EtherCATDocumentationService::generateDeveloperDocumentation() {
    QVector<DocSection> sections;

    DocSection arch;
    arch.title = QStringLiteral("Architecture");
    arch.content = QStringLiteral("The application follows a layered architecture: MainWindow partials "
                                  "for UI, services for domain logic, and EcatClient for daemon communication.");
    sections << arch;

    DocSection plugins;
    plugins.title = QStringLiteral("Plugin Development");
    plugins.content = QStringLiteral("Plugins implement the WorkspacePlugin interface and register "
                                     "themselves with the PluginRegistry. Each plugin gets access to "
                                     "the ServiceContainer for domain services.");
    sections << plugins;

    DocSection build;
    build.title = QStringLiteral("Build System");
    build.content = QStringLiteral("CMake-based build with Qt6 dependencies. Tests use QTest framework "
                                   "with AUTOMOC enabled for signal/slot support.");
    sections << build;

    return makeDoc(QStringLiteral("Developer Documentation"),
                   QStringLiteral("Technical guide for NekoEcat contributors"), sections,
                   QStringLiteral("NekoEcat Team"));
}

Documentation EtherCATDocumentationService::generateSystemDocumentation() {
    QVector<DocSection> sections;

    DocSection deploy;
    deploy.title = QStringLiteral("Deployment");
    deploy.content = QStringLiteral("NekoEcat consists of two components: ecatd (daemon) and "
                                    "ecat-studio (GUI). The daemon runs as a system service.");
    sections << deploy;

    DocSection network;
    network.title = QStringLiteral("Network Requirements");
    network.content = QStringLiteral("Requires IgH EtherCAT Master kernel modules. The daemon "
                                     "communicates with the master via the ecrt API.");
    sections << network;

    DocSection security;
    security.title = QStringLiteral("Security");
    security.content = QStringLiteral("Local TCP connection on port 5877. No authentication by default. "
                                      "Use firewall rules to restrict access in production.");
    sections << security;

    return makeDoc(QStringLiteral("System Documentation"), QStringLiteral("System administration and deployment guide"),
                   sections, QStringLiteral("NekoEcat Team"));
}

QVector<SearchResult> EtherCATDocumentationService::searchDocumentation(const QString& query) {
    QVector<SearchResult> results;

    QVector<Documentation> docs;
    docs << generateApiDocumentation() << generateUserDocumentation() << generateDeveloperDocumentation()
         << generateSystemDocumentation();

    QString lowerQuery = query.toLower();

    for (const auto& doc : docs) {
        if (doc.title.toLower().contains(lowerQuery)) {
            SearchResult r;
            r.title = doc.title;
            r.excerpt = doc.content.left(200);
            r.source = doc.title;
            r.relevance = 1.0;
            results << r;
        }

        for (const auto& section : doc.sections) {
            if (section.title.toLower().contains(lowerQuery) || section.content.toLower().contains(lowerQuery)) {
                SearchResult r;
                r.title = section.title;
                r.excerpt = section.content.left(200);
                r.source = doc.title;
                r.relevance = section.content.toLower().contains(lowerQuery) ? 0.8 : 0.6;
                results << r;
            }
        }
    }

    return results;
}
